#include "GruntController.h"

#define MIN_DISTANCE 300
#define HEALTH_LIM 25
#define ATTACK_RANGE 100

#pragma mark GruntController

void GruntController::changeStateIfApplicable(std::shared_ptr<Grunt> enemy, float distance) {
  // Change state if applicable
  int health = enemy->getHealth();
  if (health <= HEALTH_LIM) {
    enemy->setCurrentState(Grunt::State::AVOIDING);
    if (distance > MIN_DISTANCE) {
      enemy->setCurrentState(Grunt::State::IDLE);
    }
  } else {
    if (distance <= ATTACK_RANGE) {
      enemy->setCurrentState(Grunt::State::ATTACKING);
    } else if (distance <= MIN_DISTANCE) {
      enemy->setCurrentState(Grunt::State::CHASING);
    } else {
      enemy->setCurrentState(Grunt::State::IDLE);
    }
  }
}

void GruntController::performAction(std::shared_ptr<Grunt> enemy, cugl::Vec2 p) {
  switch (enemy->getCurrentState()) {
    case Grunt::State::IDLE: {
      idling(enemy);
      break;
    }
    case Grunt::State::CHASING: {
      chasePlayer(enemy, p);
      break;
    }
    case Grunt::State::ATTACKING: {
      attackPlayer(enemy, p);
      break;
    }
    case Grunt::State::AVOIDING: {
      avoidPlayer(enemy, p);
      break;
    }
  }
}
