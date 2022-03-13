#ifndef CONTROLLERS_ENEMY_CONTROLLER_H_
#define CONTROLLERS_ENEMY_CONTROLLER_H_
#include <cugl/cugl.h>

#include "../models/Player.h"
#include "../models/Grunt.h"

/**
 * A class to handle enemy AI.
 */
class EnemyController {
 private:
  /** Enum for the player's state (for animation). */
  enum State { IDLE, CHASING, ATTACKING, AVOIDING };
  
  /** Chase the player.
   *
   * @param p the player position.
   */
  void chasePlayer(cugl::Vec2 p);

  /** Attack the player.
   *
   * @param p the player position.
   */
  void attackPlayer(cugl::Vec2 p);

  /** Avoid the player.
   *
   * @param p the player position.
   */
  void avoidPlayer(cugl::Vec2 p);

  /** Idle.
   */
  void idling();

 protected:
  /** The current state of the enemy. */
  State _current_state;

  /** Reference to the enemy */
  std::shared_ptr<Grunt> _enemy;

  /** The projectile texture. */
  std::shared_ptr<cugl::Texture> _projectile_texture;
  
  /** The tile height for priorities. */
  float _tile_height;

  /** The row count for priorities. */
  float _row_count;

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
  bool init(const cugl::Vec2 pos, string name,
            std::shared_ptr<cugl::AssetManager> assets, float tile_height,
            float row_count);

  /**
   * Disposes the controller.
   */
  void dispose() {
    _enemy->dispose();
    _enemy = nullptr;
    _projectile_texture = nullptr;
  }

#pragma mark Static Constructors
  /**
   * Returns a new capsule object at the given point with no size.
   *
   * @param pos   Initial position in world coordinates.
   *
   * @return a new capsule object at the given point with no size.
   */
  static std::shared_ptr<EnemyController> alloc(
      const cugl::Vec2 pos, string name,
      std::shared_ptr<cugl::AssetManager> assets, float tile_height,
      float row_count) {
    std::shared_ptr<EnemyController> result =
        std::make_shared<EnemyController>();
    return (result->init(pos, name, assets, tile_height, row_count) ? result
                                                                    : nullptr);
  }

#pragma mark Properties

  /** Update the enemy. */
  void update(float timestep, std::shared_ptr<Player> player,
              std::shared_ptr<cugl::physics2::ObstacleWorld> _world,
              std::shared_ptr<cugl::scene2::SceneNode> _world_node,
              std::shared_ptr<cugl::scene2::SceneNode> _debug_node);
  
  /** Change the enemy state. */
  void changeStateIfApplicable(float distance);
  
  /** Perform the action according to the enemy state. */
  void performAction(cugl::Vec2 p);

  /** Update the projectiles. */
  void updateProjectiles(float timestep,
                         std::shared_ptr<cugl::physics2::ObstacleWorld> _world,
                         std::shared_ptr<cugl::scene2::SceneNode> _world_node,
                         std::shared_ptr<cugl::scene2::SceneNode> _debug_node);

  std::shared_ptr<Grunt> getEnemy() { return _enemy; }

  void deleteAllProjectiles(
      std::shared_ptr<cugl::physics2::ObstacleWorld> _world,
      std::shared_ptr<cugl::scene2::SceneNode> _world_node);
};

#endif /* CONTROLLERS_AI_CONTROLLER_H_ */
