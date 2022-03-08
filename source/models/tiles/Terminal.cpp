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
  }

  return _obstacle;
}