#include "AIController.h"

#define MIN_DISTANCE 200
#define HEALTH_LIM 25
#define ATTACK_RANGE 30

#pragma mark EnemyController

EnemyController::EnemyController(){};

//class Idle; class Chasing; class Attacking; class Avoiding;

// ----------------------------------------------------------------------------
// State: Idle
//
//class Idle : public EnemyController {
//  void entry(void) override {
//    // Add code with changing the animation state here
//  }
//
//  void react(Range const & e) override {
//    if (ATTACK_RANGE >= e.distance) {
//      transit<Attacking>();
//    } else if (e.distance < MIN_DISTANCE){
//      transit<Chasing>();
//    }
//  }
//};

void EnemyController::idling() {
  _enemy->move(0, 0);
}

// ----------------------------------------------------------------------------
// State: Chasing
//
//class Chasing : public EnemyController {
//  void entry(void) override {
//    // Add code with changing the animation state here
//  }
//
//  void react(Range const & e) override {
//    if (ATTACK_RANGE >= e.distance) {
//      transit<Attacking>();
//    }
//  }
//};

void EnemyController::chasePlayer(cugl::Vec2 p) {
  cugl::Vec2 diff = p - _enemy->getPosition();
  diff.subtract(_enemy->getVX(), _enemy->getVY()); // This is the steering velocity
//
  diff.add(_enemy->getVX(), _enemy->getVY());
  diff.scale(_enemy->getSpeed());
  _enemy->move(diff.x,diff.y);
  CULog("%f, %f", _enemy->getVX(), _enemy->getVY());
}

// ----------------------------------------------------------------------------
// State: Attacking
//
//class Attacking : public EnemyController {
//  void entry(void) override {
//    // Add code with changing the animation state here
//  }
//
//  void react(Range const & e) override {
//    if (ATTACK_RANGE < e.distance) {
//      transit<Chasing>();
//    }
//  }
//};

void EnemyController::attackPlayer() {
//  CULog("attacking");
}

// ----------------------------------------------------------------------------
// State: Avoiding
//
//class Avoiding : public EnemyController {
//  void entry(void) override {
//    // Add code with changing the animation state here
//  }
//};

void EnemyController::avoidPlayer() {
//  CULog("avoiding");
}


// ----------------------------------------------------------------------------
// Base States
//
//void EnemyController::react(Range const & e) {}
//
//void EnemyController::react(Health const & e) {
//  transit<Avoiding>();
//}

// ----------------------------------------------------------------------------
//

bool EnemyController::init(const cugl::Vec2 pos, string name, std::shared_ptr<cugl::AssetManager> assets) {
  
  std::shared_ptr<cugl::Texture> grunt_texture =
      assets->get<cugl::Texture>("grunt");
  _enemy = Grunt::alloc(pos, name);
  
  auto grunt_node = cugl::scene2::SpriteNode::alloc(grunt_texture, 1, 1);
  _enemy->setGruntNode(grunt_node);
  grunt_node->setPriority(
      100);  // TODO: Update priority according to position on screen
  
  _current_state = IDLE;
  
//  start();

  return true;
}

void EnemyController::update(float timestep, std::shared_ptr<Player> player) {
  // Calling dispatch a lot seems to cause a lot of LAG
//  Health h;
//  h.health = _enemy->getHealth();
//  Range r;
  int health = _enemy->getHealth();
  float distance = (_enemy->getPosition()).subtract(player->getPosition()).length();
  if (health <= HEALTH_LIM) {
    _current_state = AVOIDING;
    if (distance > MIN_DISTANCE) {
      _current_state = IDLE;
    }
  } else {
    if (distance <= ATTACK_RANGE) {
      _current_state = ATTACKING;
    } else if (distance <= MIN_DISTANCE) {
      _current_state = CHASING;
    } else {
      _current_state = IDLE;
    }
  }
  
  cugl::Vec2 p = player->getPosition();
  switch (_current_state) {
    case IDLE: {
      idling();
      break;
    }
    case CHASING: {
      chasePlayer(p);
      break;
    }
    case ATTACKING: {
      attackPlayer();
      break;
    }
    case AVOIDING: {
      avoidPlayer();
      break;
    }
  }
  
  _enemy->update(timestep);
}
      

//bool EnemyController::shouldMoveTowardsPlayer(std::shared_ptr<Grunt> enemy,
//                                           cugl::Vec2 playerPos) {
//  cugl::Vec2 diff = playerPos - enemy->getPosition();
//
//  return diff.length() < MIN_DISTANCE;
//}
//
//void EnemyController::moveEnemyTowardLocation(std::shared_ptr<Grunt> enemy,
//                                           cugl::Vec2 p) {
//  cugl::Vec2 diff = p - enemy->getPosition();
//
//  diff.scale(enemy->getSpeed());
//  enemy->move(diff.x, diff.y);
//}
//
//void EnemyController::moveEnemiesTowardPlayer(EnemySet enemySet,
//                                           std::shared_ptr<Player> player) {
//  std::unordered_set<std::shared_ptr<Grunt>> enemies = enemySet.getEnemies();
//  auto it = enemies.begin();
//  while (it != enemies.end()) {
//    if (shouldMoveTowardsPlayer((*it), player->getPosition())) {
//      moveEnemyTowardLocation((*it), player->getPosition());
//    }
//    ++it;
//  }
//}

//FSM_INITIAL_STATE(EnemyController, Idle); // Set the initial FSM state
