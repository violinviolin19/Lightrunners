#include "LoadingLevelScene.h"

#include <cugl/cugl.h>

#include "../generators/LevelGeneratorConfig.h"

#define SCENE_HEIGHT 720

bool LoadingLevelScene::init() {
  if (_active) return false;
  _active = true;
  // Initialize the scene to a locked width.
  cugl::Size dim = cugl::Application::get()->getDisplaySize();
  dim *= SCENE_HEIGHT / ((dim.width > dim.height) ? dim.width : dim.height);

  if (!cugl::Scene2::init(dim)) {
    return false;
  };

  _map = cugl::scene2::SceneNode::alloc();
  _map->setContentSize(200.0f, 200.0f);
  _map->setPosition(dim / 2);
  _map->setScale(cugl::Vec2::ONE);

  _level_generator = std::make_shared<LevelGenerator>();
  _level_generator->init(_config, _map);

  _map->doLayout();
  cugl::Scene2::addChild(_map);

  return true;
}

void LoadingLevelScene::dispose() {
  if (_active) return;
  _active = false;
  _level_generator = nullptr;
  _map = nullptr;
}

void LoadingLevelScene::update(float timestep) {
  cugl::Application::get()->setClearColor(cugl::Color4f::WHITE);
  if (_level_generator->update()) _active = false;
}

void LoadingLevelScene::render(
    const std::shared_ptr<cugl::SpriteBatch> &batch) {
  Scene2::render(batch);
}