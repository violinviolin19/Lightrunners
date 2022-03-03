#include "EnemySet.h"

#pragma mark EnemySet

EnemySet::EnemySet() {}

bool EnemySet::init() {
  // Reset all data
  _enemies.clear();

  return true;
}

std::shared_ptr<Grunt> EnemySet::spawnEnemy(
    cugl::Vec2 p, string n, std::shared_ptr<cugl::AssetManager> assets) {
  std::shared_ptr<cugl::Texture> grunt_texture =
      assets->get<cugl::Texture>("grunt");

  std::shared_ptr<Grunt> grunt = Grunt::alloc(p, n);
  _enemies.insert(grunt);

  auto grunt_node = cugl::scene2::SpriteNode::alloc(grunt_texture, 1, 1);
  grunt->setGruntNode(grunt_node);
  grunt_node->setPriority(100);  // TODO: Update priority according to position on screen

  return grunt;
}

void EnemySet::update(float dt) {
  // Loop through and update each enemy in set
  auto it = _enemies.begin();
  while (it != _enemies.end()) {
    (*it)->update(dt);
    ++it;
  }
}

std::unordered_set<std::shared_ptr<Grunt>> EnemySet::getEnemies() {
  return _enemies;
}
