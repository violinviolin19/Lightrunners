#ifndef CONTROLLERS_AI_CONTROLLER_H_
#define CONTROLLERS_AI_CONTROLLER_H_
#include <cugl/cugl.h>

#include "../models/EnemySet.h"
#include "../models/Player.h"

/**
 * A class to handle enemy AI.
 */
class AIController {
 private:
  /*
   * Moves the singular enemy toward the location specified
   *
   * @param enemy set of enemies to move
   * @param p position to move enemy towards
   */
  void moveEnemyTowardLocation(std::shared_ptr<Grunt> enemy, cugl::Vec2 p);

 public:
  /**
   * Creates a new AI controller with the default settings.
   */
  AIController();

  /**
   * Disposses this input controller, releasing all resources.
   */
  ~AIController() {}
  
  /*
   * Returns whether an enemy should move towards the player.
   *
   * @return whether the enemy should move towards the player.
   */
  bool shouldMoveTowardsPlayer(std::shared_ptr<Grunt> enemy, cugl::Vec2 playerPos);

  /*
   * Moves all the enemies in the enemy set towards the player character.
   *
   * @param enemies set of enemies to move
   * @param player player for enemies to move towards
   */
  void moveEnemiesTowardPlayer(EnemySet enemySet,
                               std::shared_ptr<Player> player);
};

#endif /* CONTROLLERS_AI_CONTROLLER_H_ */
