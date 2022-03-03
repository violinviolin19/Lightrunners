#include "AIController.h"

#pragma mark AIController

AIController::AIController(){};

void AIController::moveEnemyTowardLocation(std::shared_ptr<Grunt> enemy,
                                           cugl::Vec2 p) {
  cugl::Vec2 diff = p - enemy->getPosition();

  diff.scale(enemy->getSpeed());
  enemy->move(diff.x, diff.y);
}

void AIController::moveEnemiesTowardPlayer(EnemySet enemySet,
                                           std::shared_ptr<Player> player) {
  std::unordered_set<std::shared_ptr<Grunt>> enemies = enemySet.getEnemies();
  auto it = enemies.begin();
  while (it != enemies.end()) {
    moveEnemyTowardLocation((*it), player->getPosition());
    ++it;
  }
}