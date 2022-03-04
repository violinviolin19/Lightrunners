#include "Grunt.h"

#define WIDTH 24.0f
#define HEIGHT 48.0f

#define HEIGHT_SHRINK 0.3f

#pragma mark Init
/**
 * Initializes a new grunt with the given position, size, and name.
 *
 * The grunt size is specified in world coordinates.
 *
 * The scene graph is completely decoupled from the physics system.
 * The node does not have to be the same size as the physics body. We
 * only guarantee that the scene graph node is positioned correctly
 * according to the drawing scale.
 *
 * @param  pos      Initial position in world coordinates
 * @param  size       The dimensions of the box.
 * @param name The name of the grunt
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
bool Grunt::init(const cugl::Vec2 pos, string name) {
  cugl::Vec2 pos_ = pos;
  cugl::Size size_ = cugl::Size(WIDTH, HEIGHT);

  size_.height *= HEIGHT_SHRINK;
  _offset_from_center.y = HEIGHT / 2.0f - size_.height / 2.0f;
  pos_ -= _offset_from_center;

  CapsuleObstacle::init(pos_, size_);

  setName(name);

  _grunt_node = nullptr;
  _health = 100;
  _facing_left = false;
  _speed = .0005f;

  setDensity(0.01f);
  setFriction(0.0f);
  setRestitution(0.01f);
  setFixedRotation(true);

  return true;
}

void Grunt::takeDamage() {
  reduceHealth(20);
  _grunt_node->setColor(cugl::Color4::RED);
  _damage_count = 10;
}

#pragma mark Animation & Drawing

/**
 * Sets the grunt scene graph node.
 *
 * @param node the node to set it to.
 */
void Grunt::setGruntNode(
    const std::shared_ptr<cugl::scene2::SpriteNode>& node) {
  _grunt_node = node;
}

/**
 * Gets the grunt scene graph node.
 *
 * @return node the node that has been set.
 */
std::shared_ptr<cugl::scene2::SpriteNode>& Grunt::getGruntNode() {
  return _grunt_node;
}

#pragma mark -
#pragma mark Physics Methods

void Grunt::createFixtures() {
  if (_body == nullptr) return;

  CapsuleObstacle::createFixtures();

  b2FixtureDef sensorDef;
  sensorDef.density = 0.0f;
  sensorDef.isSensor = true;
  std::string fixture_name = "grunt_hitbox";
  CULog("grunt: %p", &fixture_name);
  sensorDef.userData.pointer = reinterpret_cast<uintptr_t>(&fixture_name);

  // Sensor dimensions
  b2Vec2 corners[4];
  corners[0].x = -CapsuleObstacle::getWidth() / 2.0f;
  corners[0].y = (-CapsuleObstacle::getHeight() + HEIGHT) / 2.0f;
  corners[1].x = -CapsuleObstacle::getWidth() / 2.0f;
  corners[1].y = (-CapsuleObstacle::getHeight() - HEIGHT) / 2.0f;
  corners[2].x = CapsuleObstacle::getWidth() / 2.0f;
  corners[2].y = (-CapsuleObstacle::getHeight() - HEIGHT) / 2.0f;
  corners[3].x = CapsuleObstacle::getWidth() / 2.0f;
  corners[3].y = (-CapsuleObstacle::getHeight() + HEIGHT) / 2.0f;

  b2PolygonShape sensorShape;
  sensorShape.Set(corners, 4);

  sensorDef.shape = &sensorShape;
  _hitbox_sensor = _body->CreateFixture(&sensorDef);
}

void Grunt::releaseFixtures() {
  if (_body == nullptr) return;

  CapsuleObstacle::releaseFixtures();
  if (_hitbox_sensor != nullptr) {
    _body->DestroyFixture(_hitbox_sensor);
    _hitbox_sensor = nullptr;
  }
}

void Grunt::update(float delta) {
  CapsuleObstacle::update(delta);
  if (_grunt_node != nullptr) {
    _grunt_node->setPosition(getPosition() + _offset_from_center);
  }

  if (_damage_count <= 0) {
    _grunt_node->setColor(cugl::Color4::WHITE);
    _damage_count = 0;
  } else {
    _damage_count--;
  }
}

#pragma mark Movement
/**
 * Moves the grunt by the specified amount.
 *
 * @param forwardX Amount to move in the x direction.
 * @param forwardY Amount to move in the y direction.
 */
void Grunt::move(float forwardX, float forwardY) {
  setVX(1000 * forwardX);
  setVY(1000 * forwardY);

  if (forwardX == 0) {
    setVX(0);
  } else {
    // set facing left appropriately if x direction has changed
    setFacingLeft(forwardX < 0);
  }

  if (forwardY == 0) setVY(0);
}

void Grunt::setFacingLeft(bool facing_left) {
  // flip texture if direction has changed
  if (_facing_left != facing_left) {
    _facing_left = facing_left;
    _grunt_node->flipHorizontal(_facing_left);
  }
}
