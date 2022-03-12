#pragma once

#ifndef MODELS_TILES_TERMINALSENSOR_H
#define MODELS_TILES_TERMINALSENSOR_H

#include <cugl/cugl.h>
#include <stdio.h>

class TerminalSensor : public cugl::physics2::BoxObstacle {
 private:
 

 public:
#pragma mark Constructors
  /**
   * Creates a grunt with the given position and data.
   *
   * @param pos The grunt position.
   * @param data The data defining the grunt.
   */
  TerminalSensor(void)
      : BoxObstacle() {}

  /**
   * Disposes the grunt.
   */
  ~TerminalSensor() { dispose(); }

  /**
   * Disposes the grunt.
   */
  void dispose() {

  }

  /**
   * Initializes a new grunt with the given position and size.
   *
   * @param  pos      Initial position in world coordinates.
   * @param  size       The dimensions of the box.
   *
   * @return  true if the obstacle is initialized properly, false otherwise.
   */
  virtual bool init();

#pragma mark Static Constructors
  /**
   * Returns a new capsule object at the given point with no size.
   *
   * @param pos   Initial position in world coordinates.
   *
   * @return a new capsule object at the given point with no size.
   */
  static std::shared_ptr<TerminalSensor> alloc(const cugl::Vec2 pos, string name) {
    std::shared_ptr<TerminalSensor> result = std::make_shared<TerminalSensor>();
    return (result->init(pos, name) ? result : nullptr);
  }

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

};
#endif /* Player_hpp */
#pragma once
