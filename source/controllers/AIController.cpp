#include "AIController.h"

#include <cugl/cugl.h>

AIController::AIController(){};

void moveEnemyTowardLocation(std::shared_ptr<Grunt> enemy, cugl::Vec2 p) {
  cugl::Vec2 diff = p - enemy->getPosition();

  // TODO turn this into some sort of variable somewhere
  float step = 0.2f;
  diff.scale(step);
  enemy->move(diff.x, diff.y);
}

void moveEnemiesTowardPlayer(EnemySet enemySet,
                             std::shared_ptr<Player> player) {
  std::unordered_set<std::shared_ptr<Grunt>> enemies = enemySet.getEnemies();
  auto it = enemies.begin();
  while (it != enemies.end()) {
    moveEnemyTowardLocation((*it), player->getPosition());
  }
}