#ifndef SCENES_GAME_SCENE_H_
#define SCENES_GAME_SCENE_H_
#include <cugl/cugl.h>

#include "../controllers/InputController.h"

class GameScene : public cugl::Scene2 {

public:
  GameScene() : cugl::Scene2() {}

  /**
   * Disposes of all resources allocated to this mode.
   */
  ~GameScene() { dispose(); }

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
  bool init(const std::shared_ptr<cugl::AssetManager> &assets);

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
};

#endif /* SCENES_GAME_SCENE_H_ */
