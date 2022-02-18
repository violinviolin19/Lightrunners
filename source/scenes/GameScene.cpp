#include "GameScene.h"

#include <cugl/cugl.h>

#define SCENE_HEIGHT 720

bool GameScene::init(const std::shared_ptr<cugl::AssetManager> &assets) {
  // Initialize the scene to a locked width.
  cugl::Size dim = cugl::Application::get()->getDisplaySize();
  dim *= SCENE_HEIGHT / dim.height;

  if (assets == nullptr || !cugl::Scene2::init(dim)) {
    return false;
  }

  return true;
}

void GameScene::dispose() {}

void GameScene::update(float timestep) {
}

void GameScene::render(const std::shared_ptr<cugl::SpriteBatch> &batch) {}
