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

bool GameScene::init(
    const std::shared_ptr<cugl::AssetManager>& assets,
    const std::shared_ptr<level_gen::LevelGenerator>& level_gen) {
  if (_active) return false;
  _active = true;

  // Initialize the scene to a locked width.
  // TODO delete after confirming networking works
  std::shared_ptr<cugl::NetworkConnection> _network = nullptr;

  cugl::Size dim = cugl::Application::get()->getDisplaySize();
  dim *= SCENE_HEIGHT / ((dim.width > dim.height) ? dim.width : dim.height);

  if (assets == nullptr || !cugl::Scene2::init(dim)) return false;

  _assets = assets;

  _world_node = _assets->get<cugl::scene2::SceneNode>("world-scene");
  _world_node->setContentSize(dim);

  _debug_node = cugl::scene2::SceneNode::alloc();
  _debug_node->setContentSize(dim);

  _level_controller =
      LevelController::alloc(_assets, _world_node, _debug_node, level_gen);
  _controllers.push_back(_level_controller->getHook());

  // Get the world from level controller and attach the listeners.
  _world = _level_controller->getWorld();
  _world->activateCollisionCallbacks(true);
  _world->onBeginContact = [this](b2Contact* contact) {
    this->beginContact(contact);
  };
  _world->beforeSolve = [this](b2Contact* contact,
                               const b2Manifold* oldManifold) {
    this->beforeSolve(contact, oldManifold);
  };

  _enemy_controller =
      EnemyController::alloc(_assets, _world, _world_node, _debug_node);

  populate(dim);

  _world_node->doLayout();

  auto ui_layer = assets->get<cugl::scene2::SceneNode>("ui-scene");
  ui_layer->setContentSize(dim);
  ui_layer->doLayout();

  auto text = ui_layer->getChildByName<cugl::scene2::Label>("health");
  std::string msg = cugl::strtool::format("Health: %d", _player->getHealth());
  text->setText(msg);
  text->setForeground(cugl::Color4::WHITE);

  cugl::Scene2::addChild(_world_node);
  cugl::Scene2::addChild(ui_layer);
  cugl::Scene2::addChild(_debug_node);
  _debug_node->setVisible(false);

  InputController::get()->init(_assets, cugl::Scene2::getBounds());

  return true;
}

void GameScene::dispose() {
  if (!_active) return;
  InputController::get()->dispose();
  _active = false;
}

void GameScene::populate(cugl::Size dim) {
  // Initialize the player with texture and size, then add to world.
  std::shared_ptr<cugl::Texture> player = _assets->get<cugl::Texture>("player");
  _player = Player::alloc(cugl::Vec2::ZERO, "Johnathan");

  _level_controller->getLevelModel()->setPlayer(_player);

  auto player_node = cugl::scene2::SpriteNode::alloc(player, 3, 10);
  _player->setPlayerNode(player_node);
  _world_node->addChild(player_node);
  _world->addObstacle(_player);

  _sword = Sword::alloc(dim / 2.0f);
  _world->addObstacle(_sword);
  _sword->setEnabled(false);

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
  _player->setDebugColor(cugl::Color4f::BLACK);
  _sword->setDebugScene(_debug_node);
  _sword->setDebugColor(cugl::Color4f::BLACK);

  std::shared_ptr<RoomModel> current_room =
      _level_controller->getLevelModel()->getCurrentRoom();
  for (std::shared_ptr<Grunt> enemy : current_room->getEnemies()) {
    enemy->setDebugColor(cugl::Color4f::BLACK);
    enemy->setDebugScene(_debug_node);
  }
}

void GameScene::update(float timestep) {
  cugl::Application::get()->setClearColor(cugl::Color4f::BLACK);

  InputController::get()->update();

  for (std::shared_ptr<Controller> controller : _controllers)
    controller->update();

  // Movement
  _player->step(timestep, InputController::get<Movement>()->getMovement(),
                InputController::get<Attack>()->isAttacking(), _sword);
  // Animation
  _player->animate(InputController::get<Movement>()->getMovement());

  std::shared_ptr<RoomModel> current_room =
      _level_controller->getLevelModel()->getCurrentRoom();
  for (std::shared_ptr<Grunt>& enemy : current_room->getEnemies()) {
    _enemy_controller->update(timestep, enemy, _player);
  }

  updateCamera(timestep);
  _world->update(timestep);

  // ===== POST-UPDATE =======

  auto ui_layer = _assets->get<cugl::scene2::SceneNode>("ui-scene");
  auto text = ui_layer->getChildByName<cugl::scene2::Label>("health");
  std::string msg = cugl::strtool::format("Health: %d", _player->getHealth());
  text->setText(msg);

  // Check for disposal
  std::vector<std::shared_ptr<Grunt>>& enemies = current_room->getEnemies();
  auto it = enemies.begin();
  while (it != enemies.end()) {
    auto enemy = *it;
    if (enemy->getHealth() <= 0) {
      enemy->deleteAllProjectiles(_world, _world_node);
      enemy->deactivatePhysics(*_world->getWorld());
      current_room->getNode()->removeChild(enemy->getGruntNode());
      _world->removeObstacle(enemy.get());
      enemy->dispose();
      it = enemies.erase(it);
    } else {
      enemy->deleteProjectile(_world, _world_node);
      ++it;
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
    fx1_name.assign(*static_cast<std::string*>(fx1_d));
  std::string fx2_name;
  if (static_cast<std::string*>(fx2_d) != nullptr)
    fx2_name.assign(*static_cast<std::string*>(fx2_d));

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

  if (fx1_name == "grunt_damage" && ob2 == _player.get()) {
    dynamic_cast<Player*>(ob2)->takeDamage();
  } else if (fx2_name == "grunt_damage" && ob1 == _player.get()) {
    dynamic_cast<Player*>(ob1)->takeDamage();
  }

  if (ob1->getName() == "projectile" && ob2 == _player.get()) {
    dynamic_cast<Projectile*>(ob1)->setFrames(0);  // Destroy the projectile
    dynamic_cast<Player*>(ob2)->takeDamage();
  } else if (ob2->getName() == "projectile" && ob1 == _player.get()) {
    dynamic_cast<Player*>(ob1)->takeDamage();
    dynamic_cast<Projectile*>(ob2)->setFrames(0);  // Destroy the projectile
  }

  if (ob1->getName() == "projectile" && ob2->getName() == "Wall") {
    dynamic_cast<Projectile*>(ob1)->setFrames(0);  // Destroy the projectile
  } else if (ob2->getName() == "projectile" && ob1->getName() == "Wall") {
    dynamic_cast<Projectile*>(ob2)->setFrames(0);  // Destroy the projectile
  }

  if (fx1_name.find("door") != std::string::npos && ob2 == _player.get()) {
    _level_controller->changeRoom(fx1_name);
  } else if (fx2_name.find("door") != std::string::npos &&
             ob1 == _player.get()) {
    _level_controller->changeRoom(fx2_name);
  }
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
