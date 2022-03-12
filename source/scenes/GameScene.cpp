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
  if (_active) return false;
  _active = true;
  // Initialize the scene to a locked width.
  // TODO delete after confirming networking works
  std::shared_ptr<cugl::NetworkConnection> _network = nullptr;

  cugl::Size dim = cugl::Application::get()->getDisplaySize();
  dim *= SCENE_HEIGHT / ((dim.width > dim.height) ? dim.width : dim.height);

  if (assets == nullptr || !cugl::Scene2::init(dim)) {
    return false;
  }
  _assets = assets;

  _world_node = assets->get<cugl::scene2::SceneNode>("world-scene");
  _world_node->setContentSize(dim);

  std::shared_ptr<cugl::scene2::SceneNode> grid_node =
      _world_node->getChildByName("tiles");
  std::shared_ptr<cugl::scene2::Layout> layout = grid_node->getLayout();
  std::shared_ptr<cugl::scene2::GridLayout> grid_layout =
      dynamic_pointer_cast<cugl::scene2::GridLayout>(layout);

  float map_height = grid_node->getContentHeight();
  float map_width = grid_node->getContentWidth();
  _col_count = grid_layout->getGridSize().width;
  _row_count = grid_layout->getGridSize().height;
  _tile_height = map_height / _row_count;
  _tile_width = map_width / _col_count;

  _debug_node = cugl::scene2::SceneNode::alloc();
  _debug_node->setContentSize(dim);

  // Create the world and attach the listeners.
  _world = cugl::physics2::ObstacleWorld::alloc(
      cugl::Rect(0, 0, grid_node->getContentWidth(),
                 grid_node->getContentHeight()),
      cugl::Vec2(0, 0));
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
  //  _ai_controller.~AIController(); TODO FIX THIS LATER
  _active = false;
}

void GameScene::populate(cugl::Size dim) {
  // Initialize the player with texture and size, then add to world.
  std::shared_ptr<cugl::Texture> player = _assets->get<cugl::Texture>("player");
  _player = Player::alloc(dim + cugl::Vec2(20, 20), "Johnathan");

  auto player_node = cugl::scene2::SpriteNode::alloc(player, 3, 10);
  _player->setPlayerNode(player_node);
  _world_node->addChild(player_node);
  _world->addObstacle(_player);

  _sword = Sword::alloc(dim / 2.0f);
  _world->addObstacle(_sword);
  _sword->setEnabled(false);

  // Initialize the enemy set and populate with grunts.

  std::shared_ptr<cugl::scene2::SceneNode> enemies_node =
      _world_node->getChildByName("enemies");
  std::vector<std::shared_ptr<cugl::scene2::SceneNode>> enemy_nodes =
      enemies_node->getChildren();
  for (std::shared_ptr<cugl::scene2::SceneNode> enemy_node : enemy_nodes) {
    std::shared_ptr<EnemyController> ai = EnemyController::alloc(
        enemy_node->getPosition(), enemy_node->getName(), _assets, _tile_height,
        _row_count, _world, _world_node, _debug_node);
    _e_controllers.emplace(
        std::pair<int, std::shared_ptr<EnemyController>>(_id_counter++, ai));
    _world_node->addChild(ai->getEnemy()->getGruntNode());
    _world->addObstacle(ai->getEnemy());
  }

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
  for (auto e : _e_controllers) {
    auto enemy = e.second;
    enemy->getEnemy()->setDebugColor(cugl::Color4f::BLACK);
    enemy->getEnemy()->setDebugScene(_debug_node);
  }
}

void GameScene::update(float timestep) {
  cugl::Application::get()->setClearColor(cugl::Color4f::BLACK);

  InputController::get()->update();
  // Movement
  _player->step(timestep, InputController::get<Movement>()->getMovement(),
                InputController::get<Attack>()->isAttacking(), _sword);

  int row = (int)floor(_player->getBody()->GetPosition().y / _tile_height);
  _player->getPlayerNode()->setPriority(_row_count - row);

  // Update the AI controllers
  auto it = _e_controllers.begin();
  while (it != _e_controllers.end()) {
    auto enemy = (*it->second);
    enemy.update(timestep, _player);
    ++it;
  }

  updateCamera(timestep);
  _world->update(timestep);

  auto ui_layer = _assets->get<cugl::scene2::SceneNode>("ui-scene");
  auto text = ui_layer->getChildByName<cugl::scene2::Label>("health");
  std::string msg = cugl::strtool::format("Health: %d", _player->getHealth());
  text->setText(msg);

  // Animation
  _player->animate(InputController::get<Movement>()->getMovement());

  // POST-UPDATE
  // Check for disposal
  auto itt = _e_controllers.begin();
  while (itt != _e_controllers.end()) {
    auto enemy = (*itt->second);
    if (enemy.getEnemy()->getHealth() <= 0) {
      enemy.getEnemy()->deleteAllProjectiles(_world, _world_node);
      enemy.getEnemy()->deactivatePhysics(*_world->getWorld());
      _world_node->removeChild(enemy.getEnemy()->getGruntNode());
      _world->removeObstacle(enemy.getEnemy().get());
      enemy.dispose();
      itt = _e_controllers.erase(itt);
    } else {
      ++itt;
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
