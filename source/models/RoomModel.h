#ifndef MODELS_ROOM_MODEL_H_
#define MODELS_ROOM_MODEL_H_

#include "EnemySet.h"
#include "Grunt.h"

class RoomModel {
  std::shared_ptr<cugl::scene2::SceneNode> _node;

  std::shared_ptr<EnemySet> _enemies;

  std::unordered_map<std::string, std::string> _door_sensor_id_to_room_id;

  std::unordered_map<std::string, cugl::Vec2>
      _door_sensor_id_to_destination_pos;

  std::string _name;

  cugl::Size _grid_size;

 public:
  RoomModel() {}
  ~RoomModel() { dispose(); }

  bool init(const std::shared_ptr<cugl::scene2::SceneNode>& node,
            std::string& name);

  void dispose();

  static std::shared_ptr<RoomModel> alloc(
      const std::shared_ptr<cugl::scene2::SceneNode>& node, std::string& name) {
    std::shared_ptr<RoomModel> result = std::make_shared<RoomModel>();
    return (result->init(node, name) ? result : nullptr);
  }

  void addConnection(std::string& door_sensor_id, std::string& room_id,
                     cugl::Vec2& destination) {
    if (_door_sensor_id_to_room_id.find(door_sensor_id) ==
        _door_sensor_id_to_room_id.end()) {
      _door_sensor_id_to_room_id[door_sensor_id] = room_id;
      _door_sensor_id_to_destination_pos[door_sensor_id] = destination;
    }
  }

  std::string getRoomIdFromDoorSensorId(std::string& door_sensor_id) {
    if (_door_sensor_id_to_room_id.find(door_sensor_id) !=
        _door_sensor_id_to_room_id.end()) {
      return _door_sensor_id_to_room_id[door_sensor_id];
    }
    return "";
  }

  cugl::Vec2 getPosOfDestinationDoor(std::string& door_sensor_id) {
    if (_door_sensor_id_to_destination_pos.find(door_sensor_id) !=
        _door_sensor_id_to_destination_pos.end()) {
      return _door_sensor_id_to_destination_pos[door_sensor_id];
    }
    return cugl::Vec2::ZERO;
  }

  void setVisible(bool val) { _node->setVisible(val); }

  std::shared_ptr<cugl::scene2::SceneNode> getNode() const { return _node; }

  std::string getName() const { return _name; }

  cugl::Size getGridSize() const { return _grid_size; }

  void setEnemies(const std::shared_ptr<EnemySet>& enemies) {
    _enemies = enemies;
  }

  std::shared_ptr<EnemySet> getEnemies() const { return _enemies; }
};

#endif  // MODELS_ROOM_MODEL_H_