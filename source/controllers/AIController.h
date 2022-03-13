#ifndef CONTROLLERS_AI_CONTROLLER_H_
#define CONTROLLERS_AI_CONTROLLER_H_
#include <cugl/cugl.h>

#include "../models/EnemySet.h"
#include "../models/Player.h"

//#include "../../include/tinyfsm.hpp"

/*
 * Event declarations for changing between states for this enemy.
 */
//struct Range : tinyfsm::Event { int distance; }; // Change from chasing to attacking / vice versa
//struct Health : tinyfsm::Event { int health ; }; // Change from attacking/chasing to running away

/**
 * A class to handle enemy AI.
 */
class EnemyController {
private:
  /** Enum for the player's state (for animation). */
  enum State { IDLE, CHASING, ATTACKING, AVOIDING };
//public:
//  friend class Fsm;
//private:
//  /* default reaction for unhandled events */
//  void react(tinyfsm::Event const &) { };
//
//  virtual void react(Range        const &);
//  virtual void react(Health       const &);
//
//  virtual void entry(void) { };  /* entry actions in some states */
//  void         exit(void)  { };  /* no exit actions at all */

protected:

  State _current_state;
  
  std::shared_ptr<Grunt> _enemy;
  
//  /*
//   * Moves the singular enemy toward the location specified
//   *
//   * @param enemy set of enemies to move
//   * @param p position to move enemy towards
//   */
//  void moveEnemyTowardLocation(std::shared_ptr<Grunt> enemy, cugl::Vec2 p);
  
  void chasePlayer(cugl::Vec2 p);
  
  void attackPlayer();
  
  void avoidPlayer();
  
  void idling();

 public:
  
#pragma mark Constructors
  /**
   * Creates a new AI controller with the default settings.
   */
  EnemyController();

  /**
   * Disposses this input controller, releasing all resources.
   */
  ~EnemyController() {}
  
  /**
   * Initializes a new grunt with the given position and size.
   *
   * @param  pos      Initial position in world coordinates.
   * @param  size       The dimensions of the box.
   *
   * @return  true if the obstacle is initialized properly, false otherwise.
   */
  bool init(const cugl::Vec2 pos, string name, std::shared_ptr<cugl::AssetManager> assets);
  
#pragma mark Static Constructors
  /**
   * Returns a new capsule object at the given point with no size.
   *
   * @param pos   Initial position in world coordinates.
   *
   * @return a new capsule object at the given point with no size.
   */
  static std::shared_ptr<EnemyController> alloc(const cugl::Vec2 pos, string name, std::shared_ptr<cugl::AssetManager> assets) {
    std::shared_ptr<EnemyController> result = std::make_shared<EnemyController>();
    return (result->init(pos, name, assets) ? result : nullptr);
  }
  
#pragma mark Properties
  
  /** Update the mutant. */
  void update(float timestep, std::shared_ptr<Player> player);
  
  std::shared_ptr<Grunt> getEnemy() { return _enemy; }
//
//  /*
//   * Returns whether an enemy should move towards the player.
//   *
//   * @return whether the enemy should move towards the player.
//   */
//  bool shouldMoveTowardsPlayer(std::shared_ptr<Grunt> enemy, cugl::Vec2 playerPos);
//
//  /*
//   * Moves all the enemies in the enemy set towards the player character.
//   *
//   * @param enemies set of enemies to move
//   * @param player player for enemies to move towards
//   */
//  void moveEnemiesTowardPlayer(EnemySet enemySet,
//                               std::shared_ptr<Player> player);
};

#endif /* CONTROLLERS_AI_CONTROLLER_H_ */
