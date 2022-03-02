#include "Wall.h"

bool Wall::initWithData(const cugl::Scene2Loader* loader,
                        const std::shared_ptr<cugl::JsonValue>& data) {
  if (_texture != nullptr) {
    CUAssertLog(false, "%s is already initialized", _classname.c_str());
    return false;
  }

  if (!data) return TexturedNode::init();
  if (!TexturedNode::initWithData(loader, data)) return false;

  BoxObstacle::init(TexturedNode::getPosition(),
                    TexturedNode::getContentSize());
  BoxObstacle::setName(_classname.c_str());
  BoxObstacle::setBodyType(b2BodyType::b2_staticBody);

  return true;
}