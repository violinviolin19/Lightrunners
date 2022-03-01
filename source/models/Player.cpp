#include "Player.h"

#pragma mark Init
/**
 * Initializes a new player with the given position, size, and name.
 *
 * @param  pos      Initial position in world coordinates
 * @param  size       The dimensions of the box.
 * @param name The name of the player
 *
 * @return  true if the obstacle is initialized properly, false otherwise.
 */
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

/**
 * Update the scene graph.
 *
 * @param delta the timing value
 */
void Player::update(float delta) {
    CapsuleObstacle::update(delta);
    if (_player_node != nullptr) {
        _player_node->setPosition(getPosition());
    }
}

#pragma mark Movement
/**
 * Moves the player by the specified amount.
 *
 * @param forwardX Amount to move in the x direction
 * @param forwardY Amount to move in the y direction
 */
void Player::move(float forwardX, float forwardY) {
//    _force.set(Vec2(forwardX * 5000, forwardY * 500)); //change 5 to be the player force constant from json later
//    _body->ApplyForceToCenter(b2Vec2(_force.x, _force.y), true);
    setVX(1000*forwardX);
    setVY(1000*forwardY);
    if (forwardX == 0) setVX(0);
    if (forwardY == 0) setVY(0);
}
