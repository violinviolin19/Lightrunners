#include "BasicTile.h"

bool BasicTile::initWithData(const cugl::Scene2Loader* loader,
                             const std::shared_ptr<JsonValue>& data) {
  if (_texture != nullptr) {
    CUAssertLog(false, "%s is already initialized", _classname.c_str());
    return false;
  }

  if (!data) return init();

  return TexturedNode::initWithData(loader, data);
}