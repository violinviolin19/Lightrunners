#include "LevelModel.h"

bool LevelModel::init() { return true; }

void LevelModel::dispose() {
  _current_room = nullptr;
  _player = nullptr;
  _rooms.clear();
}