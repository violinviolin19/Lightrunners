#include "TerminalSensor.h"

void TerminalSensor::createFixtures() {
  if (_body == nullptr) return;

  BoxObstacle::createFixtures();

  if (_hitbox_sensor == nullptr) {
    b2FixtureDef sensorDef;
    sensorDef.density = 0.0f;
    sensorDef.isSensor = true;
    _hitbox_sensor_name = std::make_shared<std::string>("grunt_hitbox");
    sensorDef.userData.pointer =
        reinterpret_cast<uintptr_t>(_hitbox_sensor_name.get());

    // Sensor dimensions
    b2Vec2 corners[4];
    corners[0].x = -BoxObstacle::getWidth() / 2.0f;
    corners[0].y = HEIGHT;
    corners[1].x = -BoxObstacle::getWidth() / 2.0f;
    corners[1].y = -HEIGHT / 2.0f;
    corners[2].x = BoxObstacle::getWidth() / 2.0f;
    corners[2].y = -HEIGHT / 2.0f;
    corners[3].x = BoxObstacle::getWidth() / 2.0f;
    corners[3].y = HEIGHT;

    b2PolygonShape sensorShape;
    sensorShape.Set(corners, 4);

    sensorDef.shape = &sensorShape;
    _hitbox_sensor = _body->CreateFixture(&sensorDef);
  }

  
}

void TerminalSensor::releaseFixtures() {
  if (_body == nullptr) return;

  BoxObstacle::releaseFixtures();
  if (_hitbox_sensor != nullptr) {
    _body->DestroyFixture(_hitbox_sensor);
    _hitbox_sensor = nullptr;
  }

  if (_damage_sensor != nullptr) {
    _body->DestroyFixture(_damage_sensor);
    _damage_sensor = nullptr;
  }
}