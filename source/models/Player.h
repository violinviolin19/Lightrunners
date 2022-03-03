#ifndef MODELS_PLAYER_H_
#define MODELS_PLAYER_H_

#include <cugl/cugl.h>
#include <stdio.h>

class Player : public cugl::physics2::CapsuleObstacle {
 private:
  /** Enum for the player's state (for animation). */
  enum State { IDLE, MOVING, ATTACKING };

  /** The scene graph node for the player (moving). */
  std::shared_ptr<cugl::scene2::SpriteNode> _player_node;

  /** The player's current state. */
  State _current_state;

  /** Player health. */
  int _health;

  /** Force to be applied to the player. */
  cugl::Vec2 _force;

  /** Countdown to change animation frame. */
  int _frame_count;
  
  /** Represents the offset between the center of the player and the center of
   * the capsule obstacle. */
  cugl::Vec2 _offset_from_center;

 public:
#pragma mark Constructors
  /**
   * Creates a player with the given position and data.
   *
   * @param pos The player position
   * @param data The data defining the player
   */
  Player(void) : CapsuleObstacle() {}

  /**
   * Disposes the player.
   */
  ~Player() {}

  /**
   * Initializes a new player with the given position and size.
   *
   * @param  pos      Initial position in world coordinates.
   * @param  size       The dimensions of the box.
   *
   * @return  true if the obstacle is initialized properly, false otherwise.
   */
  virtual bool init(const cugl::Vec2 pos, string name);

#pragma mark Static Constructors
  /**
   * Returns a new capsule object at the given point with no size.
   *
   * @param pos   Initial position in world coordinates.
   *
   * @return a new capsule object at the given point with no size.
   */
  static std::shared_ptr<Player> alloc(const cugl::Vec2 pos, string name) {
    std::shared_ptr<Player> result = std::make_shared<Player>();
    return (result->init(pos, name) ? result : nullptr);
  }

#pragma mark Properties

  /**
   * Returns the current state of the player.
   *
   * @return the current state.
   */
  State getState() const { return _current_state; }

  /**
   * Sets the current state of the player and changes textures accordingly.
   *
   * @param state current state.
   */
  void setState(State state) { _current_state = state; }

  /**
   * Returns the current health of the player.
   *
   * @return the current health.
   */
  int getHealth() const { return _health; }

  /**
   * Sets the current player's health.
   *
   * @param value The current player health.
   */
  void setHealth(int value) { _health = value; }

  /**
   * Update the scene graph.
   *
   * @param delta the timing value.
   */
  void update(float delta);

#pragma mark Graphics

  /**
   * Sets the scene graph node representing this player.
   *
   * @param node  The scene graph node representing this player.
   */
  void setPlayerNode(const std::shared_ptr<cugl::scene2::SpriteNode> &node) {
    _player_node = node;
  }

  /**
   * Sets the frame of the animation.
   *
   * @param forward Amount to move in the x and y direction
   */
  void move(cugl::Vec2 forward) { move(forward.x, forward.y); }

  /**
   * Moves the player by the specified amount.
   *
   * @param forwardX Amount to move in the x direction
   * @param forwardY Amount to move in the y direction
   */
  void animate(float forwardX, float forwardY);

#pragma mark Movement
  /**
   * Moves the player by the specified amount.
   *
   * @param forwardX Amount to move in the x direction.
   * @param forwardY Amount to move in the y direction.
   */
  void move(float forwardX, float forwardY);
};
#endif /* PLAYER_H */
