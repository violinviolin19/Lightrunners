#ifndef SCENES_GAME_SCENE_H_
#define SCENES_GAME_SCENE_H_
#include <box2d/b2_world_callbacks.h>
#include <cugl/cugl.h>

#include "../controllers/Controller.h"
#include "../controllers/EnemyController.h"
#include "../controllers/InputController.h"
#include "../controllers/LevelController.h"
#include "../generators/LevelGenerator.h"
#include "../models/Player.h"

class GameScene : public cugl::Scene2 {
  /** The asset manager for loading. */
  std::shared_ptr<cugl::AssetManager> _assets;

  /** The player.  */
  std::shared_ptr<Player> _player;

  /** The sword. */
  std::shared_ptr<Sword> _sword;

  /** Reference to the physics root of the scene graph. */
  std::shared_ptr<cugl::scene2::SceneNode> _world_node;

  /** Reference to the debug root of the scene graph */
  std::shared_ptr<cugl::scene2::SceneNode> _debug_node;

  /** The Box2d world */
  std::shared_ptr<cugl::physics2::ObstacleWorld> _world;

  /** The list of AI Controllers for enemies. */
  std::unordered_map<int, std::shared_ptr<EnemyController>> _e_controllers;

  /** The level controller for the game*/
  std::shared_ptr<LevelController> _level_controller;

  /** The */
  std::vector<std::shared_ptr<Controller>> _controllers;

  /** The height of each tile in the world. */
  float _tile_width;

  /** The height of each tile in the world. */
  float _tile_height;

  /** The number of rows in the world. */
  float _col_count;

  /** The number of rows in the world. */
  float _row_count;

  /** The counter for ids for the enemies */
  int _id_counter = 0;

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
   * @param assets    The (loaded) assets for this game mode.
   * @param level_gen The generated level.
   *
   * @return true if the controller is initialized properly, false otherwise.
   */
  bool init(const std::shared_ptr<cugl::AssetManager>& assets,
            const std::shared_ptr<level_gen::LevelGenerator>& level_gen);

  /**
   * Sets whether debug mode is active.
   *
   * If true, all objects will display their physics bodies.
   *
   * @param value whether debug mode is active.
   */
  void setDebug(bool value) { _debug_node->setVisible(value); }

  /**
   * Populate the scene with the Box2D objects.
   *
   * @param dim The dimensions of the screen.
   */
  void populate(cugl::Size dim);

  /**
   * The method called to update the game mode.
   * This method contains any gameplay code that is not an OpenGL call.
   *
   * @param timestep  The amount of time (in seconds) since the last frame.
   */
  void update(float timestep) override;

  /**
   * Draws all this scene to the given SpriteBatch.
   *
   * @param batch     The SpriteBatch to draw with.
   */
  void render(const std::shared_ptr<cugl::SpriteBatch>& batch) override;

#pragma mark Collision Handling
  /**
   * Processes the start of a collision.
   *
   * @param  contact  The two bodies that collided.
   */
  void beginContact(b2Contact* contact);

  /**
   * Handles any modifications necessary before collision resolution.
   *
   * @param  contact  The two bodies that collided.
   * @param  contact  The collision manifold before contact.
   */
  void beforeSolve(b2Contact* contact, const b2Manifold* oldManifold);

  /**
   * The method called to update the camera in terms of the player position.
   *
   * @param timestep The amount of time (in seconds) since the last frame.
   */
  void updateCamera(float timestep);
};

#endif /* SCENES_GAME_SCENE_H_ */
