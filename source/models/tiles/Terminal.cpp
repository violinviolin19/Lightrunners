#include "Terminal.h"

bool Terminal::initWithData(const cugl::Scene2Loader* loader,
                            const std::shared_ptr<cugl::JsonValue>& data) {
  if (_texture != nullptr) {
    CUAssertLog(false, "%s is already initialized", _classname.c_str());
    return false;
  }

  if (!data) return BasicTile::init();
  return BasicTile::initWithData(loader, data);
}

std::shared_ptr<cugl::physics2::BoxObstacle> Terminal::initBox2d() {
  _obstacle = cugl::physics2::BoxObstacle::alloc(
      BasicTile::getPosition(),
      BasicTile::getContentSize() * BasicTile::getScale());

  if (_obstacle != nullptr) {
    _obstacle->setPosition(BasicTile::getPosition());
    _obstacle->setName(_classname.c_str());
    _obstacle->setBodyType(b2BodyType::b2_staticBody);
    _obstacle->setSensor(true);
   
    _obstacle->createFixtures();

    if (_terminal_sensor == nullptr) {
      b2FixtureDef sensorDef;
      sensorDef.density = 0.0f;
      sensorDef.isSensor = true;
      _terminal_sensor_name = std::make_shared<std::string>("terminal_range");

      b2Vec2 corners[4];
      corners[0].x = -_obstacle->getWidth() / 2.0f;
      corners[0].y = _obstacle->getHeight() / 2.0f;
      corners[1].x = -_obstacle->getWidth() / 2.0f;
      corners[1].y = -_obstacle->getHeight() / 2.0f;
      corners[2].x = _obstacle->getWidth() / 2.0f;
      corners[2].y = -_obstacle->getHeight() / 2.0f;
      corners[3].x = _obstacle->getWidth() / 2.0f;
      corners[3].y = _obstacle->getHeight() / 2.0f;

      b2PolygonShape sensorShape;
      sensorShape.Set(corners, 4);

      sensorDef.shape = &sensorShape;
      _terminal_sensor = _obstacle->getBody()->CreateFixture(&sensorDef);
    }
  }

  return _obstacle;
}