#include "Player.h"

#define IDLE_RIGHT 0
#define IDLE_LEFT 1
#define IDLE_DOWN 2
#define IDLE_UP 3
#define ATTACK_LOW_LIM 10
#define ATTACK_HIGH_LIM 16
#define RUN_LOW_LIM 20
#define RUN_HIGH_LIM 29
#define ATTACK_FRAMES 14
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
    _sword = cugl::physics2::CapsuleObstacle::alloc(cugl::Vec2(pos.x + 40, pos.y + 10), cugl::Size(20, 20));
    _sword->setBodyType(b2_staticBody);
//    _sword->setSensor(true);
    
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
  switch (_current_state) {
    case MOVING: {
      // Reverse texture if moving opposite direction.
      bool movingLeft = (forwardX < 0) ? true : false;
      if (!_player_node->isFlipHorizontal() && movingLeft) {
        _player_node->flipHorizontal(true);
      } else if (_player_node->isFlipHorizontal() && !movingLeft) {
        _player_node->flipHorizontal(false);
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
    case IDLE: {
      if (_player_node->isFlipHorizontal()) {
        _player_node->setFrame(10 - IDLE_LEFT);
      } else {
        _player_node->setFrame(IDLE_RIGHT);
      }
      _frame_count = 0;
      break;
    }
      case ATTACKING: {
          if (_frame_count == 0) {
              if (_player_node->isFlipHorizontal()) {
                  _player_node->setFrame(9 + ATTACK_LOW_LIM);
              } else {
                  _player_node->setFrame(ATTACK_LOW_LIM);
              }
          }

          // Play the next animation frame.
          if (_frame_count >= 2) {
            _frame_count = 0;

            if (_player_node->isFlipHorizontal()) {
              if (_player_node->getFrame() <= 19 - ATTACK_HIGH_LIM + ATTACK_LOW_LIM) {
                _player_node->setFrame(9 + ATTACK_LOW_LIM);
              } else {
                _player_node->setFrame(_player_node->getFrame() - 1);
              }
            } else {
              if (_player_node->getFrame() >= ATTACK_HIGH_LIM) {
                _player_node->setFrame(ATTACK_LOW_LIM);
              } else {
                _player_node->setFrame(_player_node->getFrame() + 1);
              }
            }
          }
          _frame_count++;
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
    
    // Set sword position to adjacent to the player.
    if (_player_node->isFlipHorizontal()) {
        _sword->setPosition(cugl::Vec2(getPosition().x - 40, getPosition().y + 10));
    } else {
        _sword->setPosition(cugl::Vec2(getPosition().x + 40, getPosition().y + 10));
    }
    
    // Switch states.
    if (forwardX != 0 || forwardY != 0) {
      setState(MOVING);
    } else {
      setState(IDLE);
    }
}

void Player::attack(bool didAttack) {
    if (didAttack || _attack_frame_count < ATTACK_FRAMES) {
        if (_attack_frame_count == ATTACK_FRAMES) {
            _frame_count = 0;
        }
        setState(ATTACKING);
//        _sword->setEnabled(true);
        _attack_frame_count--;
    }
    
    if (_attack_frame_count <= 0) {
        setState(IDLE);
        _frame_count = 0;
//        _sword->setEnabled(false);
        _attack_frame_count = ATTACK_FRAMES;
    }
}
