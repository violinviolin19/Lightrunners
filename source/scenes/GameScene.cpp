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
  std::string msg = cugl::strtool::format("Health: %d", _my_player->getHealth());
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
  InputController::get()->dispose();
  _ai_controller.~AIController();
}

void GameScene::populate(cugl::Size dim) {
  // Initialize the player with texture and size, then add to world.
  std::shared_ptr<cugl::Texture> player = _assets->get<cugl::Texture>("player");
  
  _my_player = Player::alloc(dim + cugl::Vec2(20, 20), "Johnathan");
  _players.push_back(_my_player);

  auto player_node = cugl::scene2::SpriteNode::alloc(player, 3, 10);
  _my_player->setPlayerNode(player_node);
  _world_node->addChild(player_node);
  _world->addObstacle(_my_player);

  _sword = Sword::alloc(dim / 2.0f);
  _world->addObstacle(_sword);
  _sword->setEnabled(false);

  // Initialize the enemy set and populate with grunts.
  _enemies.init();

  std::shared_ptr<cugl::scene2::SceneNode> enemies_node =
      _world_node->getChildByName("enemies");
  std::vector<std::shared_ptr<cugl::scene2::SceneNode>> enemy_nodes =
      enemies_node->getChildren();
  for (std::shared_ptr<cugl::scene2::SceneNode> enemy_node : enemy_nodes) {
    std::shared_ptr<Grunt> grunt = _enemies.spawnEnemy(
        enemy_node->getPosition(), enemy_node->getName(), _assets);
    _world_node->addChild(grunt->getGruntNode());
    _world->addObstacle(grunt);
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
  _my_player->setDebugScene(_debug_node);
  _my_player->setDebugColor(cugl::Color4(cugl::Color4::BLACK));
  _sword->setDebugScene(_debug_node);
  _sword->setDebugColor(cugl::Color4(cugl::Color4::BLACK));
  for (std::shared_ptr<Grunt> grunt : _enemies.getEnemies()) {
    grunt->setDebugScene(_debug_node);
    grunt->setDebugColor(cugl::Color4(cugl::Color4::BLACK));
  }
}

void GameScene::update(float timestep) {
  if (_network) {
      sendNetworkInfo();

      _network->receive([this](const std::vector<uint8_t>& data) {
          processData(data);
      });
      checkConnection();
  }
  
  cugl::Application::get()->setClearColor(cugl::Color4f::BLACK);

  InputController::get()->update();
  // Movement
  _my_player->step(timestep, InputController::get<Movement>()->getMovement(),
                InputController::get<Attack>()->isAttacking(), _sword);

  int row = (int)floor(_my_player->getBody()->GetPosition().y / _tile_height);
  _my_player->getPlayerNode()->setPriority(_row_count - row);

  _ai_controller.moveEnemiesTowardPlayer(_enemies, _my_player);
  _enemies.update(timestep);

  updateCamera(timestep);
  _world->update(timestep);

  auto ui_layer = _assets->get<cugl::scene2::SceneNode>("ui-scene");
  auto text = ui_layer->getChildByName<cugl::scene2::Label>("health");
  std::string msg = cugl::strtool::format("Health: %d", _my_player->getHealth());
  text->setText(msg);

  // Animation
  _my_player->animate(InputController::get<Movement>()->getMovement());

  // POST-UPDATE
  // Check for disposal
  for (std::shared_ptr<Grunt> grunt : _enemies.getEnemies()) {
    int row = (int)floor(grunt->getBody()->GetPosition().y / _tile_height);
    grunt->getGruntNode()->setPriority(_row_count - row);

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

void GameScene::sendNetworkInfo() {
  if (auto player_id = _network->getPlayerID()) {
    _my_player->setPlayerId(*player_id);
  }
  if (_ishost) {
    std::vector<std::shared_ptr<cugl::JsonValue>> player_positions;
    for (std::shared_ptr<Player> player: _players) {
      std::shared_ptr<cugl::JsonValue> player_info = cugl::JsonValue::allocObject();

      std::shared_ptr<cugl::JsonValue> player_id = cugl::JsonValue::alloc(static_cast<long>(player->getPlayerId()));
      player_info->appendChild(player_id);
      player_id->setKey("player_id");
      
      std::shared_ptr<cugl::JsonValue> pos = cugl::JsonValue::allocArray();
      std::shared_ptr<cugl::JsonValue> pos_x = cugl::JsonValue::alloc(player->getPosition().x);
      std::shared_ptr<cugl::JsonValue> pos_y = cugl::JsonValue::alloc(player->getPosition().y);
      pos->appendChild(pos_x);
      pos->appendChild(pos_y);
      player_info->appendChild(pos);
      pos->setKey("position");

      player_positions.push_back(player_info);
    }
    
    // Send all player and enemy information.
    _serializer.writeSint32(2);
    _serializer.writeJsonVector(player_positions);
    std::vector<uint8_t> msg = _serializer.serialize();
    _serializer.reset();
    _network->send(msg);
  } else {
    // Send just the player information.
    std::shared_ptr<cugl::JsonValue> player_info = cugl::JsonValue::allocObject();

    std::shared_ptr<cugl::JsonValue> player_id = cugl::JsonValue::alloc(static_cast<long>(_my_player->getPlayerId()));
    player_info->appendChild(player_id);
    player_id->setKey("player_id");
    
    std::shared_ptr<cugl::JsonValue> pos = cugl::JsonValue::allocArray();
    std::shared_ptr<cugl::JsonValue> pos_x = cugl::JsonValue::alloc(_my_player->getPosition().x);
    std::shared_ptr<cugl::JsonValue> pos_y = cugl::JsonValue::alloc(_my_player->getPosition().y);
    pos->appendChild(pos_x);
    pos->appendChild(pos_y);
    player_info->appendChild(pos);
    pos->setKey("position");
    
    // Send individual player information.
    _serializer.writeSint32(4);
    _serializer.writeJson(player_info);
    std::vector<uint8_t> msg = _serializer.serialize();
    _serializer.reset();
    _network->send(msg);
  }
}

/**
 * Processes data sent over the network.
 *
 * Once connection is established, all data sent over the network consistes of
 * byte vectors. This function is a call back function to process that data.
 * Note that this function may be called *multiple times* per animation frame,
 * as the messages can come from several sources.
 *
 * This is where we handle the gameplay. All connected devices should immediately
 * change their color when directed by the following method. Changing the color
 * means changing the clear color of the entire {@link Application}.
 *
 * @param data  The data received
 */
void GameScene::processData(const std::vector<uint8_t>& data) {
  _deserializer.receive(data);
  Sint32 code = std::get<Sint32>(_deserializer.read());
  if (code == 2) {  // Player info update
    cugl::NetworkDeserializer::Message msg = _deserializer.read();
    std::vector<std::shared_ptr<cugl::JsonValue>> player_positions = std::get<std::vector<std::shared_ptr<cugl::JsonValue>>>(msg);
    for (std::shared_ptr<cugl::JsonValue> player: player_positions) {
      int player_id = player->getInt("player_id");
      std::shared_ptr<cugl::JsonValue> player_position = player->get("position");
      float pos_x = player_position->get(0)->asFloat();
      float pos_y = player_position->get(1)->asFloat();
      CULog("player_id: %i, position: [%f, %f]", player_id, pos_x, pos_y);
    }
  }
  _deserializer.reset();
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

  if (fx1_name == "grunt_damage" && ob2 == _my_player.get()) {
    dynamic_cast<Player*>(ob2)->takeDamage();
  } else if (fx2_name == "grunt_damage" && ob1 == _my_player.get()) {
    dynamic_cast<Player*>(ob1)->takeDamage();
  }
}

/**
 * Checks that the network connection is still active.
 *
 * Even if you are not sending messages all that often, you need to be calling
 * this method regularly. This method is used to determine the current state
 * of the scene.
 *
 * @return true if the network connection is still active.
 */
bool GameScene::checkConnection() {
    switch(_network->getStatus()) {
        case cugl::NetworkConnection::NetStatus::Pending:
        case cugl::NetworkConnection::NetStatus::Connected:
        case cugl::NetworkConnection::NetStatus::Reconnecting:
            break;
        case cugl::NetworkConnection::NetStatus::RoomNotFound:
        case cugl::NetworkConnection::NetStatus::ApiMismatch:
        case cugl::NetworkConnection::NetStatus::GenericError:
        case cugl::NetworkConnection::NetStatus::Disconnected:
            disconnect();
            return false;
    }
    return true;
}

void GameScene::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold) {
}

void GameScene::render(const std::shared_ptr<cugl::SpriteBatch>& batch) {
  Scene2::render(batch);
}

void GameScene::updateCamera(float timestep) {
  cugl::Vec2 desired_position =
      _world_node->getSize() / 2.0f - _my_player->getPosition();

  cugl::Vec2 smoothed_position;
  cugl::Vec2::lerp(_world_node->getPosition(), desired_position,
                   CAMERA_SMOOTH_SPEED * timestep, &smoothed_position);

  _world_node->setPosition(smoothed_position);
  _debug_node->setPosition(smoothed_position);
}
