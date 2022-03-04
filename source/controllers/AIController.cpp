#include "AIController.h"

#define MIN_DISTANCE 200

#pragma mark AIController

AIController::AIController(){};

bool AIController::shouldMoveTowardsPlayer(std::shared_ptr<Grunt> enemy,
                                           cugl::Vec2 playerPos) {
  cugl::Vec2 diff = playerPos - enemy->getPosition();

  return diff.length() < MIN_DISTANCE;
}

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
    if (shouldMoveTowardsPlayer((*it), player->getPosition())) {
      moveEnemyTowardLocation((*it), player->getPosition());
    }
    ++it;
  }
}
