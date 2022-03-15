#include "TankController.h"

#define MIN_DISTANCE 300
#define ATTACK_RANGE 30

#pragma mark Tank Controller

TankController::TankController(){};

void TankController::attackPlayer(std::shared_ptr<Grunt> enemy, cugl::Vec2 p) {
  // TODO: add melee fixture onto the tank, for now just attack by hitting body
}

void TankController::changeStateIfApplicable(std::shared_ptr<Grunt> enemy, float distance) {
  // Change state if applicable
  if (distance <= ATTACK_RANGE) {
    enemy->setCurrentState(Grunt::State::ATTACKING);
  } else if (distance <= MIN_DISTANCE) {
    enemy->setCurrentState(Grunt::State::CHASING);
  } else {
    enemy->setCurrentState(Grunt::State::IDLE);
  }
}

void TankController::performAction(std::shared_ptr<Grunt> enemy, cugl::Vec2 p) {
  switch (enemy->getCurrentState()) {
    case Grunt::State::CHASING: {
      chasePlayer(enemy, p);
      break;
    }
    case Grunt::State::ATTACKING: {
      attackPlayer(enemy, p);
      break;
    }
    default: {
      idling(enemy);
    }
  }
}
