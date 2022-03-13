#ifndef SCENES_LEVEL_GENERATION_DEMO_SCENE_H
#define SCENES_LEVEL_GENERATION_DEMO_SCENE_H
#include <cugl/cugl.h>

#include "../generators/LevelGenerator.h"
#include "../generators/LevelGeneratorConfig.h"
#include "../models/level_gen/Room.h"

class LoadingLevelScene : public cugl::Scene2 {
 private:
  /** A level generator for this scene. */
  std::shared_ptr<level_gen::LevelGenerator> _level_generator;

  /** A level generator config for this scene. */
  level_gen::LevelGeneratorConfig _config;

  /** A reference to the scene2 map for rendering. */
  std::shared_ptr<cugl::scene2::SceneNode> _map;

  /** A reference to the assets for the game. */
  std::shared_ptr<cugl::AssetManager> _assets;

  enum {
    /** Generate Rooms using the Level Generator. */
    GENERATE_ROOMS,
    /** Load in all the used room scene2 graphs. */
    LOAD_ROOM_SCENE2,
    /** Level loader is done. */
    DONE
  } _loading_phase;

 public:
  /** Initializes the level generation scene2. */
  LoadingLevelScene() : cugl::Scene2(), _loading_phase(GENERATE_ROOMS) {}

  /** Disposes of all resources allocated to this mode. */
  ~LoadingLevelScene() { dispose(); }

  /** Get the level generator, to pass on to the game scene. */
  std::shared_ptr<level_gen::LevelGenerator> getLevelGenerator() {
    return _level_generator;
  }

  /**
   * Disposes of all (non-static) resources allocated to this mode.
   */
  void dispose() override;

  /**
   * Initializes the controller contents, and starts generating the level.
   *
   * @param assets    The (loaded) assets for this loading mode
   *
   * @return true if the controller is initialized properly, false otherwise.
   */
  bool init(const std::shared_ptr<cugl::AssetManager>& assets);

  /**
   * The method called to update the level loading mode.
   *
   * @param timestep  The amount of time (in seconds) since the last frame
   */
  void update(float timestep) override;

  /**
   * Draws all this scene to the given SpriteBatch.
   *
   * @param batch     The SpriteBatch to draw with.
   */
  void render(const std::shared_ptr<cugl::SpriteBatch>& batch) override;
};

#endif /* SCENES_LEVEL_GENERATION_DEMO_SCENE_H */
