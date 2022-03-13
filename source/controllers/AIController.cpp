#include "AIController.h"

#define MIN_DISTANCE 200
#define HEALTH_LIM 25
#define ATTACK_RANGE 30

#pragma mark EnemyController

EnemyController::EnemyController(){};

class Idle; class Chasing; class Attacking; class Avoiding;

// ----------------------------------------------------------------------------
// State: Idle
//
class Idle : public EnemyController {
  void entry(void) override {
    // Add code with changing the animation state here
  }
  
  void react(Range const & e) override {
    if (ATTACK_RANGE >= e.distance) {
      transit<Attacking>();
    } else if (e.distance < MIN_DISTANCE){
      transit<Chasing>();
    }
  }
};

void EnemyController::idling() {
  _enemy->move(0, 0);
}

// ----------------------------------------------------------------------------
// State: Chasing
//
class Chasing : public EnemyController {
  void entry(void) override {
    // Add code with changing the animation state here
  }
  
  void react(Range const & e) override {
    if (ATTACK_RANGE >= e.distance) {
      transit<Attacking>();
    }
  }
};

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
class Attacking : public EnemyController {
  void entry(void) override {
    // Add code with changing the animation state here
  }
  
  void react(Range const & e) override {
    if (ATTACK_RANGE < e.distance) {
      transit<Chasing>();
    }
  }
};

void EnemyController::attackPlayer() {
//  CULog("attacking");
}

// ----------------------------------------------------------------------------
// State: Avoiding
//
class Avoiding : public EnemyController {
  void entry(void) override {
    // Add code with changing the animation state here
  }
};

void EnemyController::avoidPlayer() {
//  CULog("avoiding");
}


// ----------------------------------------------------------------------------
// Base States
//
void EnemyController::react(Range const & e) {}

void EnemyController::react(Health const & e) {
  transit<Avoiding>();
}

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
  
  start();

  return true;
}

void EnemyController::update(float timestep, std::shared_ptr<Player> player) {
  // Reduce computations so only call dispatches when needed
  Health h;
  h.health = _enemy->getHealth();
  Range r;
  r.distance = (_enemy->getPosition()).subtract(player->getPosition()).length();
  if (h.health <= HEALTH_LIM) {
    if (!is_in_state<Avoiding>()) {
      dispatch(h);
    }
    if (r.distance > MIN_DISTANCE) {
      start();
    }
  } else {
    // Check if the distance is very close to one of the constants, call & dispatch in that case (because can probably just hit threshold to change states)
    if (r.distance < MIN_DISTANCE) {
      dispatch(r);
    } else if (!is_in_state<Idle>()) {
      start();
    }
  }
  
  cugl::Vec2 p = player->getPosition();
  // Do is_in_state checks & calls:
  if (is_in_state<Chasing>()) {
//    CULog("Chasing");
    chasePlayer(p);
  } else if (is_in_state<Attacking>()) {
//    CULog("Attacking");
    attackPlayer();
  } else if (is_in_state<Avoiding>()) {
//    CULog("Avoiding");
    avoidPlayer();
  } else {
    idling();
//    CULog("Idling");
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

FSM_INITIAL_STATE(EnemyController, Idle); // Set the initial FSM state
