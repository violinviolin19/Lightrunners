#include "BasicTile.h"

bool BasicTile::initWithData(const cugl::Scene2Loader* loader,
                             const std::shared_ptr<cugl::JsonValue>& data) {
  if (_texture != nullptr) {
    CUAssertLog(false, "%s is already initialized", _classname.c_str());
    return false;
  }

  if (!data) return init();

  return PolygonNode::initWithData(loader, data);
}