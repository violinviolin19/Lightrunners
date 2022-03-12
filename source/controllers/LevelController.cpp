#include "LevelController.h"

#include <cugl/cugl.h>

#include "../generators/LevelGenerator.h"
#include "../generators/LevelGeneratorConfig.h"
#include "Controller.h"

void LevelController::init(
    const std::shared_ptr<cugl::AssetManager> &assets,
    const std::shared_ptr<cugl::scene2::SceneNode> &world) {
  LevelGeneratorConfig config;
  _level_generator.init(config, world);
}

void LevelController::update() {}

void LevelController::dispose() { _level_generator.dispose(); }
