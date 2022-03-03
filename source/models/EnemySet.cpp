#include "EnemySet.h"

#pragma mark EnemySet

EnemySet::EnemySet() {}

bool EnemySet::init() {
  // Reset all data
  _enemies.clear();

  return true;
}

void EnemySet::spawnEnemy(cugl::Vec2 p) {
  // TODO populate with code currently in GameScene
}

void EnemySet::update(float dt) {
  // Loop through and update each enemy in set
  auto it = _enemies.begin();
  while (it != _enemies.end()) {
    (*it)->update(dt);

    // If enemy HP has reached 0, erase enemy and handle iterator
    if ((*it)->getHealth() <= 0) {
      it = _enemies.erase(it);
    } else {
      ++it;
    }
  }
}

std::unordered_set<std::shared_ptr<Grunt>> EnemySet::getEnemies() {
  return _enemies;
}