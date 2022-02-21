#include "GameScene.h"

#include "../controllers/actions/Attack.h"
#include "../controllers/actions/Movement.h"
#include <cugl/cugl.h>

#define SCENE_HEIGHT 720

bool GameScene::init(const std::shared_ptr<cugl::AssetManager> &assets) {
  // Initialize the scene to a locked width.
  cugl::Size dim = cugl::Application::get()->getDisplaySize();
  dim *= SCENE_HEIGHT / ((dim.width > dim.height) ? dim.width : dim.height);

  if (assets == nullptr || !cugl::Scene2::init(dim)) {
    return false;
  }
  _assets = assets;

  auto ui_layer = assets->get<cugl::scene2::SceneNode>("ui-scene");
  ui_layer->setContentSize(dim);
  ui_layer->doLayout();
  cugl::Scene2::addChild(ui_layer);

  InputController::get()->init(_assets, cugl::Scene2::getBounds());

  // Set color to CORNFLOWER for nostalgia, change in real game.
  cugl::Application::get()->setClearColor(cugl::Color4f::CORNFLOWER);

  return true;
}

void GameScene::update(float timestep) { InputController::get()->update(); }

void GameScene::dispose() { InputController::get()->dispose(); }

void GameScene::render(const std::shared_ptr<cugl::SpriteBatch> &batch) {
  Scene2::render(batch);
}
