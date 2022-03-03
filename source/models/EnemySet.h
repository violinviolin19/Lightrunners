#pragma once

#ifndef MODELS_ENEMYSET_H
#define MODELS_ENEMYSET_H

#include <cugl/cugl.h>
#include <stdio.h>

#include "Grunt.h"

class EnemySet {
 private:
  /** The collection of all living enemies. */
  std::unordered_set<std::shared_ptr<Grunt>> _enemies;

 public:
  /**
   * Creates an Enemy set with the default values.
   */
  EnemySet();

  /**
   * Initializes enemy data.
   *
   * @return true if initialization was successful.
   */
  bool init();

  /**
   * Updates all the enemies in the set.
   *
   * @param dt the timing value.
   */
  void update(float dt);

  /**
   * Initializes and adds an enemy to the set of enemies.
   *
   * @param p     The enemy position.
   * @param n     The enemy name.
   * @param a     The asset manager including enemy textures.
   *
   * @return a reference to the spawned enemy.
   */
  std::shared_ptr<Grunt> spawnEnemy(cugl::Vec2 p, string n,
                                    std::shared_ptr<cugl::AssetManager> a);

  /**
   * Retrieve list of enemies.
   *
   * @return unordered set of all enemies currently alive.
   */
  std::unordered_set<std::shared_ptr<Grunt>> getEnemies();
};
#endif /* MODELS_ENEMYSET_H */