#include "Door.h"

bool Door::initWithData(const cugl::Scene2Loader* loader,
                        const std::shared_ptr<cugl::JsonValue>& data) {
  if (_texture != nullptr) {
    CUAssertLog(false, "%s is already initialized", _classname.c_str());
    return false;
  }

  if (!data) return BasicTile::init();
  return BasicTile::initWithData(loader, data);
}

std::shared_ptr<cugl::physics2::BoxObstacle> Door::initBox2d(
    std::string& sensor_name) {
  cugl::Vec2 pos = BasicTile::getWorldPosition() - BasicTile::getPosition() +
                   BasicTile::getSize() / 2.0f;

  // TODO: Change this so it is no longer a hard-coded tile size value here
  _obstacle = cugl::physics2::BoxObstacle::alloc(
      pos, cugl::Vec2(64, 64) * BasicTile::getScale());

  if (_obstacle != nullptr) {
    _obstacle->setPosition(pos);
    _obstacle->setName(_classname.c_str());

    _obstacle->setBodyType(b2BodyType::b2_staticBody);
    _obstacle->setSensor(true);

    auto sensor = std::make_shared<b2FixtureDef>();
    sensor->density = 0.0f;
    sensor->isSensor = true;
    _door_sensor_name = std::make_shared<std::string>(sensor_name);
    sensor->userData.pointer =
        reinterpret_cast<uintptr_t>(_door_sensor_name.get());

    // Sensor dimensions
    b2Vec2 corners[4];
    corners[0].x = -_obstacle->getWidth() / 2.0f;
    corners[0].y = _obstacle->getHeight() / 2.0f;
    corners[1].x = -_obstacle->getWidth() / 2.0f;
    corners[1].y = -_obstacle->getHeight() / 2.0f;
    corners[2].x = _obstacle->getWidth() / 2.0f;
    corners[2].y = -_obstacle->getHeight() / 2.0f;
    corners[3].x = _obstacle->getWidth() / 2.0f;
    corners[3].y = _obstacle->getHeight() / 2.0f;

    _sensor_shape.Set(corners, 4);
    sensor->shape = &_sensor_shape;

    std::vector<std::shared_ptr<b2FixtureDef>> sensors{sensor};
    _obstacle->addSensors(sensors);
  }

  return _obstacle;
}
