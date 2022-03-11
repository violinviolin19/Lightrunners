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

std::shared_ptr<cugl::physics2::BoxObstacle> Door::initBox2d() {
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

    b2FixtureDef sensor;
    sensor.density = 0.0f;
    sensor.isSensor = true;
    _door_sensor_name = std::make_shared<std::string>("door_sensor");
    sensor.userData.pointer =
        reinterpret_cast<uintptr_t>(_door_sensor_name.get());

    b2PolygonShape shape;
    shape.SetAsBox(_obstacle->getWidth() / 2.0f, _obstacle->getHeight() / 2.0f);
    sensor.shape = &shape;

    std::vector<b2FixtureDef> sensors{sensor};
    _obstacle->addSensors(sensors);
  }

  return _obstacle;
}
