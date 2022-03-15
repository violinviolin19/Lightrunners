#include "ShotgunnerController.h"

#define MIN_DISTANCE 300
#define HEALTH_LIM 25
#define ATTACK_RANGE 150
#define TANK_RANGE 30

#pragma mark Shotgunner Controller

ShotgunnerController::ShotgunnerController(){};

void ShotgunnerController::skirtPlayer(std::shared_ptr<Grunt> enemy, cugl::Vec2 p) {
  // Basically avoid the player, but slower
  cugl::Vec2 diff = p - enemy->getPosition();
  diff.subtract(enemy->getVX(), enemy->getVY());
  diff.add(enemy->getVX(), enemy->getVY());
  diff.scale(enemy->getSpeed() / 3);
  enemy->move(-diff.x, -diff.y);

  // Attack enemy if you can
  if (enemy->getAttackCooldown() <= 0) {
    enemy->addBullet(p);
    enemy->setAttackCooldown(120);
  }
}

void ShotgunnerController::changeStateIfApplicable(std::shared_ptr<Grunt> enemy, float distance) {
  // Change state if applicable
  int health = enemy->getHealth();
  if (distance <= ATTACK_RANGE) {
    if (health <= HEALTH_LIM) {
      enemy->setCurrentState(Grunt::State::SKIRTING);
    } else {
      enemy->setCurrentState(Grunt::State::ATTACKING);
    }
  } else if (distance <= MIN_DISTANCE) {
    enemy->setCurrentState(Grunt::State::CHASING);
  } else {
    enemy->setCurrentState(Grunt::State::IDLE);
  }
}

void ShotgunnerController::performAction(std::shared_ptr<Grunt> enemy, cugl::Vec2 p) {
  switch (enemy->getCurrentState()) {
    case Grunt::State::CHASING: {
      chasePlayer(enemy, p);
      break;
    }
    case Grunt::State::ATTACKING: {
      attackPlayer(enemy, p);
      break;
    }
    case Grunt::State::SKIRTING: {
      skirtPlayer(enemy, p);
      break;
    }
    default: {
      idling(enemy);
    }
  }
}
