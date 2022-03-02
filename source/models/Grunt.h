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
  std::shared_ptr<cugl::scene2::SceneNode> _grunt_node;

  /** Force to be applied to the grunt. */
  cugl::Vec2 _force;

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
   * Disposes the grunt
   */
  ~Grunt() {}

  /**
   * Initializes a new grunt with the given position and size.
   *
   * @param  pos      Initial position in world coordinates.
   * @param  size       The dimensions of the box.
   *
   * @return  true if the obstacle is initialized properly, false otherwise.
   */
  virtual bool init(const cugl::Vec2 pos, const cugl::Size size, string name);

#pragma mark Static Constructors
  /**
   * Returns a new capsule object at the given point with no size.
   *
   * @param pos   Initial position in world coordinates.
   *
   * @return a new capsule object at the given point with no size.
   */
  static std::shared_ptr<Grunt> alloc(const cugl::Vec2 pos,
                                      const cugl::Size& size, string name) {
    std::shared_ptr<Grunt> result = std::make_shared<Grunt>();
    return (result->init(pos, size, name) ? result : nullptr);
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
  void setGruntNode(const std::shared_ptr<cugl::scene2::PolygonNode>& node);

#pragma mark Movement
  /**
   * Moves the grunt by the specified amount.
   *
   * @param forwardX Amount to move in the x direction.
   * @param forwardY Amount to move in the y direction.
   */
  void move(float forwardX, float forwardY);
};
#endif /* Player_hpp */