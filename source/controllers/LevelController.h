#ifndef CONTROLLERS_LEVEL_CONTROLLER_H_
#define CONTROLLERS_LEVEL_CONTROLLER_H_

#include <cugl/cugl.h>

#include "../generators/LevelGenerator.h"
#include "Controller.h"

/**
 * A level controller that takes care of initializing the board model.
 */
class LevelController : public Controller {
 private:
  /** A level generator. */
  LevelGenerator _level_generator;

 public:
  /** Construct a new Level Controller */
  LevelController() : _world(nullptr), _assets(nullptr) {}
  /** Destroy this Level Controller */
  ~LevelController() { dispose(); }

  /**
   * Initialize the level controller.
   * @param assets The assets for the game.
   * @param world The scene2 node for the world.
   */
  void init(const std::shared_ptr<cugl::AssetManager> &assets,
            const std::shared_ptr<cugl::scene2::SceneNode> &world) override;

  /** Update the Level Controller. */
  void update() override;

  /** Dispose of the level controller and all it's values. */
  void dispose() override;
}

#endif  // CONTROLLERS_LEVEL_CONTROLLER_H_
