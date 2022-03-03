#include "Player.h"

#define PLAYER_WIDTH_SHRINK 0.5f
#define PLAYER_HEIGHT_SHRINK 0.3f

#pragma mark Init

bool Player::init(const cugl::Vec2 pos, const cugl::Size size, string name) {
  cugl::Vec2 pos_ = pos;
  cugl::Size size_ = size;

  size_.width *= PLAYER_WIDTH_SHRINK;
  size_.height *= PLAYER_HEIGHT_SHRINK;
  _offset_from_center.y = size.height / 2.0f - size_.height / 2.0f;
  pos_ -= _offset_from_center;

  CapsuleObstacle::init(pos_, size_);
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
    _player_node->setPosition(getPosition() + _offset_from_center);
  }
}

#pragma mark Movement

void Player::move(float forwardX, float forwardY) {
  setVX(1000 * forwardX);
  setVY(1000 * forwardY);
  if (forwardX == 0) setVX(0);
  if (forwardY == 0) setVY(0);
}
