//
//  Player.hpp
//  Luminance
//
//  Created by Judy Ng on 2/21/22.
//  Copyright © 2022 Game Design Initiative at Cornell. All rights reserved.
//

#ifndef Player_hpp
#define Player_hpp

#include <stdio.h>
#include <cugl/cugl.h>

using namespace cugl;
using namespace physics2;

class Player : public CapsuleObstacle {
private:
    
    /** Player health */
    int _health;

    /** The scene graph node for the player */
    std::shared_ptr<cugl::scene2::SceneNode> _playerNode;
    
    /** Force to be applied to the player */
    Vec2 _force;
    
public:
    
#pragma mark Constructors
    /**
     * Creates a player with the given position and data
     *
     * @param pos The ship position
     * @param data The data defining the player
     */
    Player(void) : CapsuleObstacle() { }
    
    /**
     * Disposes the player
     */
    ~Player() {}
    
    /**
     * Initializes a new player with the given position and size.
     *
     * @param  pos      Initial position in world coordinates
     * @param  size       The dimensions of the box.
     *
     * @return  true if the obstacle is initialized properly, false otherwise.
     */
    virtual bool init(const Vec2 pos, const Size size, string name);
    
#pragma mark Static Constructors
    /**
     * Returns a new capsule object at the given point with no size.
     *
     * @param pos   Initial position in world coordinates
     *
     * @return a new capsule object at the given point with no size.
     */
    static std::shared_ptr<Player> alloc(const Vec2 pos, const cugl::Size& size, string name) {
        std::shared_ptr<Player> result = std::make_shared<Player>();
        return (result->init(pos, size, name) ? result : nullptr);
    }
    
#pragma mark Properties
    
    /**
     * Returns the current health of the player.
     *
     * @return the current health.
     */
    int getHealth() const { return _health; }
    
    /**
     * Sets the current player's health.
     *
     * @param value The current ship health.
     */
    void setHealth(int value) { _health = value; }
    
    /**
     * Update the scene graph.
     *
     * @param delta the timing value
     */
    void update(float delta);
    
#pragma mark Graphics
    
    /**
     * Sets the scene graph node representing this player.
     *
     * @param node  The scene graph node representing this rocket.
     */
    void setPlayerNode(const std::shared_ptr<cugl::scene2::PolygonNode>& node);
    
#pragma mark Movement
    /**
     * Moves the player by the specified amount.
     *
     * @param forwardX Amount to move in the x direction
     * @param forwardY Amount to move in the y direction
     */
    void move(float forwardX, float forwardY);
};
#endif /* Player_hpp */
