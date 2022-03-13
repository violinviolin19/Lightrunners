#include "Projectile.h"

#pragma mark Init
bool Projectile::init(const cugl::Vec2 pos, const cugl::Vec2 v) {
  CapsuleObstacle::init(pos, cugl::Size(20, 20));
  cugl::Vec2 v2 = cugl::Vec2(v * 100);
  setVX(v2.x);
  setVY(v2.y);
  setBullet(true);
  setSensor(true);
  setDensity(0.01f);
  setFriction(0.0f);
  setRestitution(0.01f);
  setFixedRotation(true);
  
  _live_frames = 60; // 1 seconds
  _in_world = false;

  return true;
}
