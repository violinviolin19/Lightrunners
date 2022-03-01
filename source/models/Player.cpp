#include "Player.h"

#pragma mark Init

bool Player::init(const cugl::Vec2 pos, const cugl::Size size, string name) {
  CapsuleObstacle::init(pos, size);
  setName(name);

  _player_node = nullptr;
  _health = 100;

  setDensity(0.01f);
  setFriction(0.0f);
  setRestitution(0.01f);
  setFixedRotation(true);

  return true;
}

#pragma mark Animation & Drawing

void Player::update(float delta) {
  CapsuleObstacle::update(delta);
  if (_player_node != nullptr) {
    _player_node->setPosition(getPosition());
  }
}

#pragma mark Movement

void Player::move(float forwardX, float forwardY) {
  setVX(1000 * forwardX);
  setVY(1000 * forwardY);
  if (forwardX == 0)
    setVX(0);
  if (forwardY == 0)
    setVY(0);
}
