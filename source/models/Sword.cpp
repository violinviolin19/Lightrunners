#include "Sword.h"

// Offset from the player
#define OFFSET_X 30
#define OFFSET_Y 5

#define SIZE 25

bool Sword::init(const cugl::Vec2 pos) {
  CapsuleObstacle::init(cugl::Vec2(pos.x + OFFSET_X, pos.y + OFFSET_Y),
                        cugl::Size(SIZE, SIZE));

  setBodyType(b2_kinematicBody);
  setDensity(0.01f);
  setFriction(0.0f);
  setRestitution(0.01f);
  setFixedRotation(true);
  setSensor(true);

  return true;
}

void Sword::moveSword(const cugl::Vec2 pos, const cugl::Vec2 vel,
                      bool isFacingLeft) {
  int change = (isFacingLeft) ? OFFSET_X * -1 : OFFSET_X;
  setPosition(pos.x + change, pos.y + OFFSET_Y);

  setVX(1000 * vel.x);
  setVY(1000 * vel.y);
}
