#pragma once

#ifndef MODELS_GRUNT_H
#define MODELS_GRUNT_H

#include <cugl/cugl.h>
#include <stdio.h>

class Grunt : public cugl::physics2::CapsuleObstacle {
 private:
  /** Grunt health. */
  int _health;

  /** Grunt speed. */
  float _speed;

  /** The scene graph node for the grunt. */
  std::shared_ptr<cugl::scene2::SpriteNode> _grunt_node;

  /** Represents the hit area for the grunt. */
  b2Fixture* _hitbox_sensor;

  /** The node for debugging the hitbox sensor */
  std::shared_ptr<cugl::scene2::WireNode> _hitbox_sensor_node;

  /** Force to be applied to the grunt. */
  cugl::Vec2 _force;

  /** Grunt direction. */
  bool _facing_left;

  /** Damage frame count to turn red. */
  int _damage_count;

  /** Represents the offset between the center of the player and the center of
   * the capsule obstacle. */
  cugl::Vec2 _offset_from_center;

 public:
#pragma mark Constructors
  /**
   * Creates a grunt with the given position and data.
   *
   * @param pos The grunt position.
   * @param data The data defining the grunt.
   */
  Grunt(void) : CapsuleObstacle(), _hitbox_sensor(nullptr) {}

  /**
   * Disposes the grunt.
   */
  ~Grunt() { dispose(); }

  /**
   * Disposes the grunt.
   */
  void dispose() {
    _grunt_node = nullptr;
    _hitbox_sensor = nullptr;
  }

  /**
   * Initializes a new grunt with the given position and size.
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
  static std::shared_ptr<Grunt> alloc(const cugl::Vec2 pos, string name) {
    std::shared_ptr<Grunt> result = std::make_shared<Grunt>();
    return (result->init(pos, name) ? result : nullptr);
  }

#pragma mark Properties

  /**
   * Returns the current health of the grunt.
   *
   * @return the current health.
   */
  int getHealth() const { return _health; }

  /**
   * Sets the current grunt's health.
   *
   * @param value The current grunt health.
   */
  void setHealth(int value) { _health = value; }

  /**
   * Reduces the grunt's health.
   *
   * @param value The value to reduce the health by.
   */
  void reduceHealth(int value) { _health -= value; }

  /**
   * The grunt took damage.
   *
   */
  void takeDamage();

  /**
   * Returns the speed of the grunt.
   *
   * @return the grunt speed.
   */
  float getSpeed() const { return _speed; }

#pragma mark -
#pragma mark Physics Methods
  /**
   * Creates the physics Body(s) for this object, adding them to the world.
   *
   * This method overrides the base method to keep your ship from spinning.
   *
   * @param world Box2D world to store body
   *
   * @return true if object allocation succeeded
   */
  void createFixtures() override;

  /**
   * Release the fixtures for this body, reseting the shape
   *
   * This is the primary method to override for custom physics objects.
   */
  void releaseFixtures() override;

  /**
   * Updates the object's physics state (NOT GAME LOGIC).
   *
   * We use this method to reset cooldowns.
   *
   * @param delta Number of seconds since last animation frame
   */
  void update(float dt) override;

#pragma mark Graphics

  /**
   * Sets the scene graph node representing this grunt.
   *
   * @param node  The scene graph node representing this grunt.
   */
  void setGruntNode(const std::shared_ptr<cugl::scene2::SpriteNode>& node);

  /**
   * Gets the grunt scene graph node.
   *
   * @return node the node that has been set.
   */
  std::shared_ptr<cugl::scene2::SpriteNode>& getGruntNode();

#pragma mark Movement
  /**
   * Moves the grunt by the specified amount.
   *
   * @param forwardX Amount to move in the x direction.
   * @param forwardY Amount to move in the y direction.
   */
  void move(float forwardX, float forwardY);

  /**
   * Changes the direction of the grunt.
   *
   * @param facing_left is true if character should face left, false otherwise.
   */
  void setFacingLeft(bool facing_left);
};
#endif /* Player_hpp */
