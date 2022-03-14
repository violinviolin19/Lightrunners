#include "LevelController.h"

#include <cugl/cugl.h>

#include "../generators/LevelGenerator.h"
#include "../generators/LevelGeneratorConfig.h"
#include "../models/RoomModel.h"
#include "../models/tiles/Door.h"
#include "Controller.h"

#define TILE_SCALE cugl::Vec2(0.6, 0.6)
#define TILE_SIZE cugl::Vec2(64, 64)

bool LevelController::init(
    const std::shared_ptr<cugl::AssetManager> &assets,
    const std::shared_ptr<cugl::scene2::SceneNode> &world_node,
    const std::shared_ptr<cugl::scene2::SceneNode> &debug_node,
    const std::shared_ptr<level_gen::LevelGenerator> &level_gen) {
  _assets = assets;
  _world_node = world_node;
  _debug_node = debug_node;
  _level_gen = level_gen;
  populate();
  return true;
}

void LevelController::update() {
  std::shared_ptr<RoomModel> current = _level_model->getCurrentRoom();
  std::shared_ptr<Player> player = _level_model->getPlayer();
  float rel_player_y =
      player->getBody()->GetPosition().y - current->getNode()->getPosition().y;
  int row = (int)floor(rel_player_y / (TILE_SIZE.y * TILE_SCALE.y));
  player->getPlayerNode()->setPriority(current->getGridSize().height - row);

  for (std::shared_ptr<Grunt> enemy : current->getEnemies()) {
    b2Body *enemy_body = enemy->getBody();
    if (enemy_body != nullptr) {
      float rel_enemy_y =
          enemy_body->GetPosition().y - current->getNode()->getPosition().y;
      row = (int)floor(rel_enemy_y / (TILE_SIZE.y * TILE_SCALE.y));
      enemy->getGruntNode()->setPriority(current->getGridSize().height - row);

      for (std::shared_ptr<Projectile> projectile : enemy->getProjectiles()) {
        float rel_projectile_y = projectile->getBody()->GetPosition().y -
                                 current->getNode()->getPosition().y;
        row = (int)floor(rel_projectile_y / (TILE_SIZE.y * TILE_SCALE.y));
        player->getPlayerNode()->setPriority(current->getGridSize().height -
                                             row);
      }
    }
  }
}

void LevelController::dispose() { _level_gen->dispose(); }

void LevelController::changeRoom(std::string &door_sensor_name) {
  std::shared_ptr<RoomModel> current = _level_model->getCurrentRoom();

  std::string destination_room_id =
      current->getRoomIdFromDoorSensorId(door_sensor_name);
  if (destination_room_id == "") return;

  cugl::Vec2 door_pos = current->getPosOfDestinationDoor(door_sensor_name);

  for (std::shared_ptr<Grunt> enemy : current->getEnemies())
    enemy->promiseToChangePhysics(false);

  current->setVisible(false);
  _level_model->setCurrentRoom(destination_room_id);

  std::shared_ptr<RoomModel> new_current =
      _level_model->getCurrentRoom();  // New current level.

  new_current->setVisible(true);
  _level_model->getPlayer()->setPosPromise(
      new_current->getNode()->getPosition() +
      door_pos * (TILE_SIZE * TILE_SCALE));

  for (std::shared_ptr<Grunt> enemy : new_current->getEnemies())
    enemy->promiseToChangePhysics(true);

  CULog("%s -> %s", current->getName().c_str(), new_current->getName().c_str());
}

void LevelController::populate() {
  // Represent the two corners of the world.
  cugl::Vec2 world_start;
  cugl::Vec2 world_end;

  _level_model = LevelModel::alloc();

  // Get Size of World.
  for (std::shared_ptr<level_gen::Room> room : _level_gen->getRooms()) {
    cugl::Vec2 pos = room->getRect().origin * (TILE_SIZE * TILE_SCALE);
    cugl::Vec2 size =
        ((cugl::Vec2)room->getRect().size) * (TILE_SIZE * TILE_SCALE);
    if (pos.x < world_start.x) world_start.x = pos.x;
    if (pos.y < world_start.y) world_start.y = pos.y;
    if (pos.x + size.x > world_end.x) world_end.x = pos.x;
    if (pos.y + size.y > world_end.y) world_end.y = pos.y;
  }

  _world = cugl::physics2::ObstacleWorld::alloc(
      cugl::Rect(world_start, world_end - world_start));

  // Initialize every room.
  for (std::shared_ptr<level_gen::Room> room : _level_gen->getRooms()) {
    auto node = _assets->get<cugl::scene2::SceneNode>(room->_scene2_key);

    node->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);
    cugl::Vec2 pos = room->getRect().origin * (TILE_SIZE * TILE_SCALE);
    node->setPosition(pos);
    node->setVisible(false);

    auto room_model = RoomModel::alloc(node, room->_scene2_key);
    _level_model->addRoom(room->_scene2_key, room_model);

    // Make spawn the starting point.
    if (room->_type == level_gen::Room::RoomType::SPAWN) {
      _level_model->setCurrentRoom(room->_scene2_key);
      node->setVisible(true);
    }

    // Initialize doors.
    for (auto &it : room->_edge_to_door) {
      std::shared_ptr<level_gen::Edge> edge = it.first;
      cugl::Vec2 door = it.second;

      std::stringstream ss;
      ss << room->_scene2_key << "_tiles_tile-("
         << room_model->getGridSize().height - static_cast<int>(door.y) - 1
         << "-" << static_cast<int>(door.x) << ")_tile";
      std::string door_name = ss.str();

      auto door_node = std::dynamic_pointer_cast<Door>(
          _assets->get<cugl::scene2::SceneNode>(door_name));

      if (door_node) {
        std::string door_sensor_name = door_name + "-door";
        _world->addObstacle(door_node->initBox2d(door_sensor_name));

        std::shared_ptr<level_gen::Room> other_room = edge->getOther(room);
        std::string other_room_id = other_room->_scene2_key;
        cugl::Vec2 destination = other_room->_edge_to_door[edge];

        if (destination.x == 0) destination.x += 1;
        if (destination.y == 0) destination.y += 3;
        if (destination.x == room_model->getGridSize().width - 1)
          destination.x -= 1;
        if (destination.y == room_model->getGridSize().height - 1)
          destination.y -= 1;

        if (other_room_id != "") {
          room_model->addConnection(door_sensor_name, other_room_id,
                                    destination);
        }
      }
    }

    std::vector<std::shared_ptr<Grunt>> enemies;
    // Initialize enemies in room.
    for (std::shared_ptr<cugl::scene2::SceneNode> enemy_node :
         node->getChildByName("enemies")->getChildren()) {
      auto grunt_texture = _assets->get<cugl::Texture>("grunt");
      std::shared_ptr<Grunt> grunt =
          Grunt::alloc(enemy_node->getWorldPosition(), enemy_node->getName());
      enemies.push_back(grunt);

      auto grunt_node = cugl::scene2::SpriteNode::alloc(grunt_texture, 1, 1);
      grunt->setGruntNode(grunt_node);

      grunt->setRoomPos(pos);
      node->addChild(grunt->getGruntNode());
      _world->addObstacle(grunt);
      if (room->_type != level_gen::Room::RoomType::SPAWN)
        grunt->setEnabled(false);

      grunt->setDebugScene(_debug_node);
      grunt->setDebugColor(cugl::Color4(cugl::Color4::BLACK));
    }
    room_model->setEnemies(enemies);

    _world_node->addChild(node);
  }
}
