#ifndef CONTROLLERS_LEVEL_CONTROLLER_H_
#define CONTROLLERS_LEVEL_CONTROLLER_H_

#include <cugl/cugl.h>

#include "../generators/LevelGenerator.h"
#include "../models/LevelModel.h"
#include "Controller.h"

/**
 * A level controller that takes care of initializing the board model.
 */
class LevelController : public Controller {
 private:
  /** A reference to all game assets. */
  std::shared_ptr<cugl::AssetManager> _assets;
  /** A reference to the world node. */
  std::shared_ptr<cugl::scene2::SceneNode> _world_node;
  /** A reference to the debug node. */
  std::shared_ptr<cugl::scene2::SceneNode> _debug_node;
  /** A reference to the box2d world. */
  std::shared_ptr<cugl::physics2::ObstacleWorld> _world;
  /** A level generator. */
  std::shared_ptr<level_gen::LevelGenerator> _level_gen;
  /** The level model. */
  std::shared_ptr<LevelModel> _level_model;

 public:
  /** Construct a new Level Controller */
  LevelController() {}
  /** Destroy this Level Controller */
  ~LevelController() { dispose(); }

  /**
   * Initialize the level controller.
   * @param assets The assets for the game.
   * @param world The scene2 node for the world.
   * @param level_gen The level generator.
   */
  bool init(const std::shared_ptr<cugl::AssetManager> &assets,
            const std::shared_ptr<cugl::scene2::SceneNode> &world_node,
            const std::shared_ptr<cugl::scene2::SceneNode> &debug_node,
            const std::shared_ptr<level_gen::LevelGenerator> &level_gen);

  /**
   * This method allocated a new level controller.
   * @param assets The assets for the game.
   * @param world The scene2 node for the world.
   * @param level_gen The level generator.
   */
  static std::shared_ptr<LevelController> alloc(
      const std::shared_ptr<cugl::AssetManager> &assets,
      const std::shared_ptr<cugl::scene2::SceneNode> &world_node,
      const std::shared_ptr<cugl::scene2::SceneNode> &debug_node,
      const std::shared_ptr<level_gen::LevelGenerator> &level_gen) {
    auto result = std::make_shared<LevelController>();
    if (result->init(assets, world_node, debug_node, level_gen)) return result;
    return nullptr;
  }

  /** Update the Level Controller. */
  void update() override;

  /** Dispose of the level controller and all it's values. */
  void dispose() override;

  /** Change room given a door that was hit.
   * @param door_sensor_name The name of the door sensor that was hit.
   */
  void changeRoom(std::string &door_sensor_name);

  /**
   * Get the box2d world.
   * @return The box2d world for the game.
   */
  std::shared_ptr<cugl::physics2::ObstacleWorld> getWorld() const {
    return _world;
  }

  /**
   * Get the level model that contains all the rooms, player, enemies, etc...
   * @return The level model.
   */
  std::shared_ptr<LevelModel> getLevelModel() const { return _level_model; }

 private:
  /** Populate the level. */
  void populate();
};

#endif  // CONTROLLERS_LEVEL_CONTROLLER_H_
