#include "GameScene.h"

#include <box2d/b2_collision.h>
#include <box2d/b2_contact.h>
#include <cugl/cugl.h>

#include "../controllers/actions/Attack.h"
#include "../controllers/actions/Movement.h"
#include "../loaders/CustomScene2Loader.h"
#include "../models/tiles/Wall.h"

#define SCENE_HEIGHT 720
#define CAMERA_SMOOTH_SPEED 2.0f

bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
  // Initialize the scene to a locked width.
  cugl::Size dim = cugl::Application::get()->getDisplaySize();
  dim *= SCENE_HEIGHT / ((dim.width > dim.height) ? dim.width : dim.height);

  if (assets == nullptr || !cugl::Scene2::init(dim)) {
    return false;
  }
  _assets = assets;

  _world_node = assets->get<cugl::scene2::SceneNode>("world-scene");
  _world_node->setContentSize(dim);

  _debug_node = cugl::scene2::SceneNode::alloc();
  _debug_node->setContentSize(dim);

  // Create the world and attach the listeners.
  _world = cugl::physics2::ObstacleWorld::alloc(
      cugl::Rect(0, 0, dim.width, dim.height), cugl::Vec2(0, 0));
  _world->activateCollisionCallbacks(true);
  _world->onBeginContact = [this](b2Contact* contact) {
    beginContact(contact);
  };
  _world->beforeSolve = [this](b2Contact* contact,
                               const b2Manifold* oldManifold) {
    beforeSolve(contact, oldManifold);
  };

  populate(dim);

  _world_node->doLayout();

  auto ui_layer = assets->get<cugl::scene2::SceneNode>("ui-scene");
  ui_layer->setContentSize(dim);
  ui_layer->doLayout();

  cugl::Scene2::addChild(_world_node);
  cugl::Scene2::addChild(ui_layer);
  cugl::Scene2::addChild(_debug_node);
  // _debug_node->setVisible(false);

  InputController::get()->init(_assets, cugl::Scene2::getBounds());

  // Set color to BLACK.
  cugl::Application::get()->setClearColor(cugl::Color4f::BLACK);

  return true;
}

void GameScene::dispose() {
  InputController::get()->dispose();
  _ai_controller.~AIController();
}

void GameScene::populate(cugl::Size dim) {
  // Initialize the player with texture and size, then add to world.
  std::shared_ptr<cugl::Texture> player = _assets->get<cugl::Texture>("player");
  _player = Player::alloc(dim / 2.0f, "Johnathan");

  auto player_node = cugl::scene2::SpriteNode::alloc(player, 3, 10);
  player_node->setPriority(
      100);  // TODO: Update priority according to position on screen
  _player->setPlayerNode(player_node);
  _world_node->addChild(player_node);
  _world->addObstacle(_player);

  _sword = Sword::alloc(dim / 2.0f);
  _world->addObstacle(_sword);
  _sword->setEnabled(false);

  // Initialize the enemy set and populate with grunts.
  _enemies.init();

  // Each grunt has texture and size set, and is added to the world.
  std::shared_ptr<Grunt> grunt1 =
      _enemies.spawnEnemy(dim / 2.3f, "Grunt1", _assets);
  _world_node->addChild(grunt1->getGruntNode());
  _world->addObstacle(grunt1);

  std::shared_ptr<Grunt> grunt2 =
      _enemies.spawnEnemy(dim / 5.7f, "Grunt2", _assets);
  _world_node->addChild(grunt2->getGruntNode());
  _world->addObstacle(grunt2);

  // Add physics enabled tiles to world node, debug node and box2d physics
  // world.
  std::shared_ptr<cugl::CustomScene2Loader> loader =
      std::dynamic_pointer_cast<cugl::CustomScene2Loader>(
          _assets->access<cugl::scene2::SceneNode>());

  for (std::shared_ptr<BasicTile> tile : loader->getTiles("wall")) {
    auto wall = std::dynamic_pointer_cast<Wall>(tile);
    _world->addObstacle(wall->initBox2d());
    wall->getObstacle()->setDebugColor(cugl::Color4::GREEN);
    wall->getObstacle()->setDebugScene(_debug_node);
  }

  // Debug code.
  _player->setDebugScene(_debug_node);
  _player->setDebugColor(cugl::Color4(cugl::Color4::BLACK));
  _sword->setDebugScene(_debug_node);
  _sword->setDebugColor(cugl::Color4(cugl::Color4::BLACK));
  for (std::shared_ptr<Grunt> grunt : _enemies.getEnemies()) {
    grunt->setDebugScene(_debug_node);
    grunt->setDebugColor(cugl::Color4(cugl::Color4::BLACK));
  }
}

void GameScene::update(float timestep) {
  InputController::get()->update();
  // Movement
  _player->move(InputController::get<Movement>()->getMovement());

  std::shared_ptr<Attack> att = InputController::get<Attack>();
  _player->attack(att->isAttacking(), _sword);

  _ai_controller.moveEnemiesTowardPlayer(_enemies, _player);
  _enemies.update(timestep);

  updateCamera(timestep);
  _world->update(timestep);

  // Animation
  _player->animate(InputController::get<Movement>()->getMovement());

  // POST-UPDATE
  // Check for disposal
  for (std::shared_ptr<Grunt> grunt : _enemies.getEnemies()) {
    if (grunt != nullptr && grunt->getHealth() <= 0) {
      grunt->deactivatePhysics(*_world->getWorld());
      _enemies.deleteEnemy(grunt);
      _world_node->removeChild(grunt->getGruntNode());
      _world->removeObstacle(grunt.get());
      grunt->dispose();
      grunt = nullptr;
    }
  }
}

void GameScene::beginContact(b2Contact* contact) {
  b2Fixture* fx1 = contact->GetFixtureA();
  b2Fixture* fx2 = contact->GetFixtureB();

  void* fx1_d = (void*)fx1->GetUserData().pointer;
  void* fx2_d = (void*)fx2->GetUserData().pointer;

  std::string fx1_name;
  if (static_cast<std::string*>(fx1_d) != nullptr)
    CULog("%p", static_cast<std::string*>(fx1_d));
  std::string fx2_name;
  if (static_cast<std::string*>(fx2_d) != nullptr)
    CULog("%p", static_cast<std::string*>(fx2_d));

  b2Body* body1 = fx1->GetBody();
  b2Body* body2 = fx2->GetBody();

  cugl::physics2::Obstacle* ob1 = static_cast<cugl::physics2::Obstacle*>(
      (void*)body1->GetUserData().pointer);
  cugl::physics2::Obstacle* ob2 = static_cast<cugl::physics2::Obstacle*>(
      (void*)body2->GetUserData().pointer);

  if (!ob1 || !ob2) return;

  if (fx1_name == "grunt_hitbox" && ob2 == _sword.get()) {
    dynamic_cast<Grunt*>(ob1)->takeDamage();
  } else if (fx2_name == "grunt_hitbox" && ob1 == _sword.get()) {
    dynamic_cast<Grunt*>(ob2)->takeDamage();
  }

  // If there is a collision between the sword and the enemies
  // for (std::shared_ptr<Grunt> grunt : _enemies.getEnemies()) {
  //   intptr_t gptr = reinterpret_cast<intptr_t>(grunt.get());
  //   if ((body1->GetUserData().pointer == psptr &&
  //        body2->GetUserData().pointer == gptr) ||
  //       (body1->GetUserData().pointer == gptr &&
  //        body2->GetUserData().pointer == psptr)) {
  //     grunt->takeDamage();
  //   }

  // If there is a collision between the player and the enemy
  //   if ((body1->GetUserData().pointer == pptr &&
  //        body2->GetUserData().pointer == gptr) ||
  //       (body1->GetUserData().pointer == gptr &&
  //        body2->GetUserData().pointer == pptr)) {
  //     _player->reduceHealth(5);
  //   }
  // }
}

void GameScene::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold) {
}

void GameScene::render(const std::shared_ptr<cugl::SpriteBatch>& batch) {
  Scene2::render(batch);
}

void GameScene::updateCamera(float timestep) {
  cugl::Vec2 desired_position =
      _world_node->getSize() / 2.0f - _player->getPosition();

  cugl::Vec2 smoothed_position;
  cugl::Vec2::lerp(_world_node->getPosition(), desired_position,
                   CAMERA_SMOOTH_SPEED * timestep, &smoothed_position);

  _world_node->setPosition(smoothed_position);
  _debug_node->setPosition(smoothed_position);
}
