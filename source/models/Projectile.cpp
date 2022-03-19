#include "Projectile.h"

#pragma mark Init
bool Projectile::init(const cugl::Vec2 pos, const cugl::Vec2 v) {
  CapsuleObstacle::init(pos, cugl::Size(5, 5));
  cugl::Vec2 v2 = cugl::Vec2(v * 10000);
  setVX(v2.x);
  setVY(v2.y);
  if (v.x == 0) {
    setVX(0);
  }
  if (v.y == 0) {
    setVY(0);
  }
  setSensor(true);
  setDensity(0.00f);
  setFriction(0.0f);
  setRestitution(0.00f);
  setFixedRotation(true);

  setName("projectile");

  _live_frames = 120;  // 1 seconds
  _in_world = false;

  return true;
}
