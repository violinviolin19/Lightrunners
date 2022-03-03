#include "Player.h"

#define WIDTH 22
#define HEIGHT 50

#define IDLE_RIGHT 0
#define IDLE_LEFT 1
#define IDLE_DOWN 2
#define IDLE_UP 3
#define ATTACK_LOW_LIM 10
#define ATTACK_HIGH_LIM 19
#define RUN_LOW_LIM 20
#define RUN_HIGH_LIM 29

#pragma mark Init

bool Player::init(const cugl::Vec2 pos, string name) {
  CapsuleObstacle::init(pos, cugl::Size(WIDTH, HEIGHT));
  setName(name);

  _player_node = nullptr;
  _current_state = IDLE;
  _health = 100;
  _frame_count = 0;

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

void Player::animate(float forwardX, float forwardY) {
  // Switch states.
  if (forwardX != 0 || forwardY != 0) {
    setState(MOVING);
  } else {
    setState(IDLE);
  }

  switch (_current_state) {
    case MOVING: {
      // Reverse texture if moving opposite direction.
      bool movingLeft = forwardX < 0;
      if (_player_node->isFlipHorizontal() != movingLeft) {
        _player_node->flipHorizontal(movingLeft);
      }

      if (_frame_count == 0) {
        _player_node->setFrame(RUN_LOW_LIM);
      }

      // Play the next animation frame.
      if (_frame_count >= 5) {
        _frame_count = 0;

        if (_player_node->isFlipHorizontal()) {
          if (_player_node->getFrame() <= RUN_LOW_LIM) {
            _player_node->setFrame(RUN_HIGH_LIM);
          } else {
            _player_node->setFrame(_player_node->getFrame() - 1);
          }
        } else {
          if (_player_node->getFrame() >= RUN_HIGH_LIM) {
            _player_node->setFrame(RUN_LOW_LIM);
          } else {
            _player_node->setFrame(_player_node->getFrame() + 1);
          }
        }
      }
      _frame_count++;
      break;
    }
    default: {
      if (_player_node->isFlipHorizontal()) {
        _player_node->setFrame(10 - IDLE_LEFT);
      } else {
        _player_node->setFrame(IDLE_RIGHT);
      }
      _frame_count = 0;
      break;
    }
  }
}

#pragma mark Movement

void Player::move(float forwardX, float forwardY) {
  setVX(1000 * forwardX);
  setVY(1000 * forwardY);
  if (forwardX == 0) setVX(0);
  if (forwardY == 0) setVY(0);
}
