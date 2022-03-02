#include "Player.h"

#pragma mark Init

bool Player::init(const cugl::Vec2 pos, const cugl::Size size, string name) {
  CapsuleObstacle::init(pos, size);
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

#pragma mark Properties

void Player::setState(State state) {
    _current_state = state;
    switch(_current_state) {
        case State::IDLE:
            _player_node->setTexture(_idle_texture);
            break;
        case MOVING:
            _player_node->setTexture(_moving_texture);
            _player_node->_cols = 5;
            break;
        case ATTACKING:
            <#code#>
            break;
    }
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
                    _player_node->setFrame(_player_node->getSize()-1);
                } else {
                    _player_node->setFrame(_player_node->getFrame()-1);
                }
            } else {
                if (_player_node->getFrame() >= _player_node->getSize()-1) {
                    _player_node->setFrame(0);
                } else {
                    _player_node->setFrame(_player_node->getFrame()+1);
                }
            }
        }
        _frame_count++;
    } else {
        _frame_count = 0;
    }
}

#pragma mark Movement

void Player::move(float forwardX, float forwardY) {
  setVX(1000 * forwardX);
  setVY(1000 * forwardY);
  if (forwardX == 0) setVX(0);
  if (forwardY == 0) setVY(0);
    
    if (forwardX != 0 || forwardY != 0) {
        setState(MOVING);
    } else {
        setState(IDLE);
    }
}
