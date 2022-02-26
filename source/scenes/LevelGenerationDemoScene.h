#ifndef SCENES_LEVEL_GENERATION_DEMO_SCENE_H
#define SCENES_LEVEL_GENERATION_DEMO_SCENE_H
#include "../generators/LevelGenerator.h"
#include "../generators/LevelGeneratorConfig.h"
#include <cugl/cugl.h>

class LevelGenerationDemoScene : public cugl::Scene2 {
private:
  LevelGenerator _level_generator;
  bool _keep_separating_rooms;
  int _ticks;
  std::shared_ptr<cugl::scene2::SceneNode> _map;
  LevelGeneratorConfig _config;

public:
  LevelGenerationDemoScene()
      : cugl::Scene2(), _keep_separating_rooms(true), _ticks(0) {}

  /**
   * Disposes of all resources allocated to this mode.
   */
  ~LevelGenerationDemoScene() { dispose(); }

  /**
   * Disposes of all (non-static) resources allocated to this mode.
   */
  void dispose() override;

  /**
   * Initializes the controller contents, and starts the game.
   *
   * @param assets    The (loaded) assets for this game mode
   *
   * @return true if the controller is initialized properly, false otherwise.
   */
  bool init();

  /**
   * The method called to update the game mode.
   * This method contains any gameplay code that is not an OpenGL call.
   *
   * @param timestep  The amount of time (in seconds) since the last frame
   */
  void update(float timestep) override;

  /**
   * Draws all this scene to the given SpriteBatch.
   *
   * @param batch     The SpriteBatch to draw with.
   */
  void render(const std::shared_ptr<cugl::SpriteBatch> &batch) override;

  void createLevel();
};

#endif /* SCENES_LEVEL_GENERATION_DEMO_SCENE_H */
