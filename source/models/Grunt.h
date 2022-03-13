#pragma once

#ifndef MODELS_GRUNT_H
#define MODELS_GRUNT_H

#include <cugl/cugl.h>
#include <stdio.h>

#include "Projectile.h"

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
  /** Keeps an instance of the name alive for collision detection. */
  std::shared_ptr<std::string> _hitbox_sensor_name;
  /** The node for debugging the hitbox sensor */
  std::shared_ptr<cugl::scene2::WireNode> _hitbox_sensor_node;

  /** Represents the hit area for the grunt. */
  b2Fixture* _damage_sensor;
  /** Keeps an instance of the name alive for collision detection. */
  std::shared_ptr<std::string> _damage_sensor_name;
  /** The node for debugging the damage sensor */
  std::shared_ptr<cugl::scene2::WireNode> _damage_sensor_node;
  
  /** The list of projectiles that have been shot by the grunt. */
  std::unordered_set<std::shared_ptr<Projectile>> _projectiles;

  /** Force to be applied to the grunt. */
  cugl::Vec2 _force;

  /** Grunt direction. */
  bool _facing_left;

  /** Damage frame count to turn red. */
  int _damage_count;
  
  /** Attack cooldown. */
  int _attack_cooldown;

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
  Grunt(void)
      : CapsuleObstacle(),
        _hitbox_sensor(nullptr),
        _hitbox_sensor_name(nullptr),
        _damage_sensor(nullptr),
        _damage_sensor_name(nullptr) {}

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
    _damage_sensor = nullptr;
    _projectiles.clear();
  }

  /**
   * Initializes a new grunt with the given position and size.
   *
   * @param  pos      Initial position in world coordinates.
   * @param  size       The dimensions of the box.
   *
   * @return  true if the obstacle is initialized properly, false otherwise.
   */
  bool init(const cugl::Vec2 pos, string name);

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
   * Gets the current attack cooldown of the grunt.
   *
   * @return the current health.
   */
  int getAttackCooldown() const { return _attack_cooldown; }

  /**
   * Sets the current grunt's health.
   *
   * @param value The current grunt health.
   */
  void setHealth(int value) { _health = value; }
  
  /**
   * Sets the attack cooldown.
   *
   * @param value The attack cooldown.
   */
  void setAttackCooldown(int value) { _attack_cooldown = value; }

  /**
   * Reduces the grunt's health.
   *
   * @param value The value to reduce the health by.
   */
  void reduceHealth(int value) { _health -= value; }
  
  /**
   * Reduces the grunt's attack cooldown.
   *
   * @param value The value to reduce the health by.
   */
  void reduceAttackCooldown(int value) { _attack_cooldown -= value; }

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
  
  /**
   * Add a bullet.
   *
   * @param p the position of the bullet to spawn in.
   */
  void addBullet(cugl::Vec2 p);
  
  /**
   * Deletes a bullet if needed.
   */
  void deleteProjectile(std::shared_ptr<cugl::physics2::ObstacleWorld> _world, std::shared_ptr<cugl::scene2::SceneNode> _world_node);
  
  /**
   * Deletes all bullets.
   */
  void deleteAllProjectiles(std::shared_ptr<cugl::physics2::ObstacleWorld> _world, std::shared_ptr<cugl::scene2::SceneNode> _world_node);
  
  /**
   * Gets the grunt's projectiles.
   *
   * @return the projectiles the grunt has shot.
   */
  std::unordered_set<std::shared_ptr<Projectile>> getProjectiles() { return _projectiles; }

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
