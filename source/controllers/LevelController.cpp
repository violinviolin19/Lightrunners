#include <cugl/cugl.h>

#include "../generators/LevelGenerator.h"
#include "Controller.h"

void init(const std::shared_ptr<cugl::AssetManager> &assets,
          const std::shared_ptr<cugl::scene2::SceneNode> &world) {}

void update() override;

void dispose() override;
