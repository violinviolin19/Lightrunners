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
   * Adds an enemy to the set of enemies.
   *
   * @param p     The enemy position.
   * @param a     The ship angle.
   */
  void spawnEnemy(cugl::Vec2 p);

  /**
   * Updates all the enemies in the set.
   *
   * @param dt the timing value.
   */
  void update(float dt);

 public:
  /**
   * Retrieve list of enemies.
   *
   * @return unordered set of all enemies currently alive.
   */
  std::unordered_set<std::shared_ptr<Grunt>> getEnemies();
};
#endif /* MODELS_ENEMYSET_H */