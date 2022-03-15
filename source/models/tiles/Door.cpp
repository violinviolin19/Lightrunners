#include "Door.h"

bool Door::initWithData(const cugl::Scene2Loader* loader,
                        const std::shared_ptr<cugl::JsonValue>& data) {
  if (_texture != nullptr) {
    CUAssertLog(false, "%s is already initialized", _classname.c_str());
    return false;
  }

  if (!data) return BasicTile::init();

  _horizontal = data->getBool("horizontal", true);

  return BasicTile::initWithData(loader, data);
}

std::shared_ptr<cugl::physics2::BoxObstacle> Door::initBox2d(
    std::string& sensor_name) {
  cugl::Vec2 pos = BasicTile::getWorldPosition() - BasicTile::getPosition();

  cugl::Size size = BasicTile::getSize();

  if (_horizontal) {
    size.height *= 0.2f;
    pos.x += BasicTile::getSize().width / 2;
    pos.y += BasicTile::getSize().height / 4;
  } else {
    size.width *= 0.2f;
    pos += BasicTile::getSize() / 2;
  }
  _obstacle = cugl::physics2::BoxObstacle::alloc(pos, size);

  if (_obstacle != nullptr) {
    _obstacle->setPosition(pos);
    _obstacle->setName(_classname.c_str());

    _obstacle->setBodyType(b2BodyType::b2_staticBody);

    auto sensor = std::make_shared<b2FixtureDef>();
    sensor->density = 0.0f;
    sensor->isSensor = true;
    _door_sensor_name = std::make_shared<std::string>(sensor_name);
    sensor->userData.pointer =
        reinterpret_cast<uintptr_t>(_door_sensor_name.get());

    // Sensor dimensions
    _sensor_shape.SetAsBox(BasicTile::getSize().width / 2,
                           BasicTile::getSize().height / 2);
    sensor->shape = &_sensor_shape;

    std::vector<std::shared_ptr<b2FixtureDef>> sensors{sensor};
    _obstacle->addSensors(sensors);
  }

  return _obstacle;
}
