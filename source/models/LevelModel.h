#ifndef MODELS_LEVEL_MODEL_H_
#define MODELS_LEVEL_MODEL_H_

#include "Player.h"
#include "RoomModel.h"

class LevelModel {
  std::shared_ptr<RoomModel> _current_room;

  std::unordered_map<std::string, std::shared_ptr<RoomModel>> _rooms;

  std::shared_ptr<Player> _player;

 public:
  LevelModel() : _current_room(nullptr), _player(nullptr) {}
  ~LevelModel() { dispose(); }

  bool init();

  void dispose();

  static std::shared_ptr<LevelModel> alloc() {
    std::shared_ptr<LevelModel> result = std::make_shared<LevelModel>();
    return (result->init() ? result : nullptr);
  }

  void addRoom(std::string& unique_id, const std::shared_ptr<RoomModel>& room) {
    // If Room Id doesn't already exists.
    if (_rooms.find(unique_id) == _rooms.end()) _rooms[unique_id] = room;
  }

  void setCurrentRoom(std::string& room_id) {
    if (_rooms.find(room_id) == _rooms.end()) return;
    _current_room = _rooms[room_id];
  }

  std::shared_ptr<RoomModel> getCurrentRoom() const { return _current_room; }

  std::unordered_map<std::string, std::shared_ptr<RoomModel>> getRooms() const {
    return _rooms;
  }

  void setPlayer(const std::shared_ptr<Player>& player) { _player = player; }
  std::shared_ptr<Player> getPlayer() const { return _player; }
};

#endif  // MODELS_LEVEL_MODEL_H_