#include "TurtleController.h"

#define MIN_DISTANCE 300
#define HEALTH_LIM 25
#define ATTACK_RANGE 100
#define TANK_RANGE 30

#pragma mark Turtle Controller

TurtleController::TurtleController(){};

void TurtleController::attackPlayer(std::shared_ptr<Grunt> enemy, const cugl::Vec2 p) {
  if (enemy->getAttackCooldown() <= 0) {
    cugl::Vec2 e = enemy->getPosition();

    // Attack in closest cardinal direction
    if (abs(p.x - e.x) > abs(p.y - e.y)) {
      if (p.x - e.x > 0) {
        enemy->addBullet(cugl::Vec2(e.x + p.x, e.y));
      } else {
        enemy->addBullet(cugl::Vec2(e.x - p.x, e.y));
      }
    } else {
      if (p.y - e.y > 0) {
        enemy->addBullet(cugl::Vec2(e.x, e.y + p.y));
      } else {
        enemy->addBullet(cugl::Vec2(e.x, e.y - p.y));
      }
    }
    enemy->setAttackCooldown(120);
  }
  enemy->move(0, 0);
}

void TurtleController::tank(std::shared_ptr<Grunt> enemy) {
  enemy->move(0, 0);
}

void TurtleController::changeStateIfApplicable(std::shared_ptr<Grunt> enemy, float distance) {
  if (distance <= TANK_RANGE) {
    enemy->setCurrentState(Grunt::State::TANKING);
  } else if (distance <= ATTACK_RANGE) {
    enemy->setCurrentState(Grunt::State::ATTACKING);
  } else {
    enemy->setCurrentState(Grunt::State::IDLE);
  }
}

void TurtleController::performAction(std::shared_ptr<Grunt> enemy, cugl::Vec2 p) {
  switch (enemy->getCurrentState()) {
    case Grunt::State::ATTACKING: {
      attackPlayer(enemy, p);
      break;
    }
    case Grunt::State::TANKING: {
      tank(enemy);
      break;
    }
    default: {
      idling(enemy);
    }
  }
}
