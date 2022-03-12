#include "TerminalSensor.h"

bool TerminalSensor::init(const cugl::Vec2 pos, string name) {
  cugl::Vec2 pos_ = pos;
  cugl::Size size_ = cugl::Size(64.0f, 64.0f);

  BoxObstacle::init(pos_, size_);

  setDensity(0.01f);
  setFriction(0.0f);
  setRestitution(0.01f);
  setFixedRotation(true);
  setSensor(true);

  return true;
}

void TerminalSensor::createFixtures() {
  if (_body == nullptr) return;

  BoxObstacle::createFixtures();

  if (_terminal_sensor == nullptr) {
    b2FixtureDef sensorDef;
    sensorDef.density = 0.0f;
    sensorDef.isSensor = true;
    _terminal_sensor_name = std::make_shared<std::string>("terminal_range");
    sensorDef.userData.pointer =
        reinterpret_cast<uintptr_t>(_terminal_sensor_name.get());

    // Sensor dimensions
    b2Vec2 corners[4];
    corners[0].x = -BoxObstacle::getWidth();
    corners[0].y = BoxObstacle::getWidth();
    corners[1].x = -BoxObstacle::getWidth();
    corners[1].y = -BoxObstacle::getWidth();
    corners[2].x = BoxObstacle::getWidth();
    corners[2].y = -BoxObstacle::getWidth();
    corners[3].x = BoxObstacle::getWidth();
    corners[3].y = BoxObstacle::getWidth();

    b2PolygonShape sensorShape;
    sensorShape.Set(corners, 4);

    sensorDef.shape = &sensorShape;
    _terminal_sensor = _body->CreateFixture(&sensorDef);
  }
 
}

void TerminalSensor::releaseFixtures() {
  if (_body == nullptr) return;

  BoxObstacle::releaseFixtures();
  if (_terminal_sensor != nullptr) {
    _body->DestroyFixture(_terminal_sensor);
    _terminal_sensor = nullptr;
  }

}