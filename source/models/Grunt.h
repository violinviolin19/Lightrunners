#pragma once

#ifndef MODELS_GRUNT_H
#define MODELS_GRUNT_H

#include <cugl/cugl.h>
#include <stdio.h>

class Grunt : public cugl::physics2::CapsuleObstacle {
 private:
  /** Grunt health. */
  int _health;

  /** The scene graph node for the grunt. */
  std::shared_ptr<cugl::scene2::SpriteNode> _grunt_node;

  /** Force to be applied to the grunt. */
  cugl::Vec2 _force;

  /** Grunt direction. */
  bool _facing_left;

  /** Damage frame count to turn red. */
  int _damage_count;

 public:
#pragma mark Constructors
  /**
   * Creates a grunt with the given position and data.
   *
   * @param pos The grunt position.
   * @param data The data defining the grunt.
   */
  Grunt(void) : CapsuleObstacle() {}

  /**
   * Disposes the grunt.
   */
  ~Grunt() { dispose(); }

  /**
   * Disposes the grunt.
   */
  void dispose() { _grunt_node = nullptr; }

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
   * Update the scene graph.
   *
   * @param delta the timing value.
   */
  void update(float delta);

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
