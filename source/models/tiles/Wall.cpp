#include "Wall.h"

bool Wall::initWithData(const cugl::Scene2Loader* loader,
                        const std::shared_ptr<cugl::JsonValue>& data) {
  if (_texture != nullptr) {
    CUAssertLog(false, "%s is already initialized", _classname.c_str());
    return false;
  }

  if (!data) return BasicTile::init();
  return BasicTile::initWithData(loader, data);
}

std::shared_ptr<cugl::physics2::BoxObstacle> Wall::initBox2d() {
  cugl::Vec2 pos = BasicTile::getWorldPosition() - BasicTile::getPosition() +
                   BasicTile::getSize() / 2.0f;

  // TODO: Change this so it is no longer a hard-coded tile size value here
  _obstacle = cugl::physics2::BoxObstacle::alloc(
      pos, cugl::Vec2(64, 64) * BasicTile::getScale());

  if (_obstacle != nullptr) {
    _obstacle->setPosition(pos);
    _obstacle->setName(_classname.c_str());

    _obstacle->setBodyType(b2BodyType::b2_staticBody);
  }

  return _obstacle;
}
