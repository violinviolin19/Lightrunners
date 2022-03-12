#ifndef GAMEAPP_H_
#define GAMEAPP_H_
#include <cugl/cugl.h>

#include "scenes/GameScene.h"
#include "scenes/LoadingLevelScene.h"
#include "scenes/LoadingScene.h"
#include "scenes/MenuScene.h"

/**
 * This class represents the application root for the game.
 */
class GameApp : public cugl::Application {
 protected:
  /** The global sprite batch for drawing (only want one of these) */
  std::shared_ptr<cugl::SpriteBatch> _batch;
  /** The global asset manager */
  std::shared_ptr<cugl::AssetManager> _assets;

  enum Scene {
    /** Currently loading the loading scene. */
    LOADING,
    /** Currently rendering the menu scene. */
    MENU,
    /** Currently loading the level loading scene. */
    LEVEL_LOADING,
    /** Currently loading the game scene. */
    GAME,
  } _current_scene;

  /** The primary controller for the game world */
  GameScene _gameplay;
  /** The controller for the loading screen */
  LoadingScene _loading;
  /** The controller for the main menu */
  MenuScene _menu;
  /** The controller for the level generation loading scene. */
  LoadingLevelScene _level_loading;

  /** Whether or not we have finished loading all assets */
  bool _loaded;
  /** Whether or not we have finished loading the level */
  bool _level_loaded;

 public:
  GameApp()
      : cugl::Application(),
        _loaded(false),
        _level_loaded(false),
        _current_scene(Scene::LOADING) {}
  ~GameApp() {}

#pragma mark Application State
  /**
   * The method called after OpenGL is initialized, but before running the
   * application.
   *
   * This is the method in which all user-defined program intialization should
   * take place.  You should not create a new init() method.
   *
   * When overriding this method, you should call the parent method as the
   * very last line.  This ensures that the state will transition to FOREGROUND,
   * causing the application to run.
   */
  virtual void onStartup() override;

  /**
   * The method called when the application is ready to quit.
   *
   * This is the method to dispose of all resources allocated by this
   * application.  As a rule of thumb, everything created in onStartup()
   * should be deleted here.
   *
   * When overriding this method, you should call the parent method as the
   * very last line.  This ensures that the state will transition to NONE,
   * causing the application to be deleted.
   */
  virtual void onShutdown() override;

#pragma mark Application Loop
  /**
   * The method called to update the application data.
   *
   * This is your core loop and should be replaced with your custom
   * implementation. This method should contain any code that is not an OpenGL
   * call.
   *
   * When overriding this method, you do not need to call the parent method
   * at all. The default implmentation does nothing.
   *
   * @param timestep  The amount of time (in seconds) since the last frame
   */
  virtual void update(float timestep) override;

  /**
   * The method called to draw the application to the screen.
   *
   * This is your core loop and should be replaced with your custom
   * implementation. This method should OpenGL and related drawing calls.
   *
   * When overriding this method, you do not need to call the parent method
   * at all. The default implmentation does nothing.
   */
  virtual void draw() override;
};

#endif /* GAMEAPP_H_ */
