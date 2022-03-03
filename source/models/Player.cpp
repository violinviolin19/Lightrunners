#include "Player.h"

#define ATTACK_FRAMES 10
#define HEALTH 100

#pragma mark Init

bool Player::init(const cugl::Vec2 pos, const cugl::Size size, string name) {
  CapsuleObstacle::init(pos, size);
  setName(name);

  _player_node = nullptr;
  _current_state = IDLE;
  _health = HEALTH;
  _frame_count = 0;
    _attack_frame_count = ATTACK_FRAMES;

  setDensity(0.01f);
  setFriction(0.0f);
  setRestitution(0.01f);
  setFixedRotation(true);

    // Sword init
    _sword = cugl::physics2::CapsuleObstacle::alloc(cugl::Vec2(pos.x + 15, pos.y), size);
    _sword->setBodyType(b2_staticBody);
    _sword->setSensor(true);
    _sword->setEnabled(false);
  return true;
}

#pragma mark Animation & Drawing

void Player::update(float delta) {
  CapsuleObstacle::update(delta);
  if (_player_node != nullptr) {
    _player_node->setPosition(getPosition());
  }
}

void Player::animate(float forwardX, float forwardY) {
  if (_current_state == MOVING) {
    // Reverse texture if moving opposite direction
    bool movingLeft = (forwardX < 0) ? true : false;
    if (!_player_node->isFlipHorizontal() && movingLeft) {
      _player_node->flipHorizontal(true);
    } else if (_player_node->isFlipHorizontal() && !movingLeft) {
      _player_node->flipHorizontal(false);
    }

    // Play the next animation frame
    if (_frame_count >= 5) {
      _frame_count = 0;
      if (_player_node->isFlipHorizontal()) {
        if (_player_node->getFrame() <= 0) {
          _player_node->setFrame(_player_node->getSize() - 1);
        } else {
          _player_node->setFrame(_player_node->getFrame() - 1);
        }
      } else {
        if (_player_node->getFrame() >= _player_node->getSize() - 1) {
          _player_node->setFrame(0);
        } else {
          _player_node->setFrame(_player_node->getFrame() + 1);
        }
      }
    }
    _frame_count++;
  } else {
    if (_player_node->isFlipHorizontal()) {
      _player_node->setFrame(_player_node->getSize() - 1);
    } else {
      _player_node->setFrame(0);
    }
    _frame_count = 0;
  }
}

#pragma mark Movement

void Player::move(float forwardX, float forwardY) {
  setVX(1000 * forwardX);
  setVY(1000 * forwardY);
  if (forwardX == 0) setVX(0);
  if (forwardY == 0) setVY(0);
    
    // Set sword position to adjacent to the player
    if (_player_node->isFlipHorizontal()) {
        _sword->setPosition(cugl::Vec2(getPosition().x - 15, getPosition().y));
    } else {
        _sword->setPosition(cugl::Vec2(getPosition().x + 15, getPosition().y));
    }

  if (forwardX != 0 || forwardY != 0) {
    setState(MOVING);
  } else {
    setState(IDLE);
  }
}

void Player::attack(bool didAttack) {
    if (didAttack || _attack_frame_count < ATTACK_FRAMES) {
        setState(ATTACKING);
        _sword->setEnabled(true);
        _attack_frame_count--;
    }
    
    if (_attack_frame_count <= 0) {
        setState(IDLE);
        _sword->setEnabled(false);
        _attack_frame_count = ATTACK_FRAMES;
    }
}
