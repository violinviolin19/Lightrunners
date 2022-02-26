#include "LevelGenerationDemoScene.h"
#include "../generators/LevelGeneratorConfig.h"

#include <cugl/cugl.h>

#define SCENE_HEIGHT 720

bool LevelGenerationDemoScene::init() {
  // Initialize the scene to a locked width.
  cugl::Size dim = cugl::Application::get()->getDisplaySize();
  dim *= SCENE_HEIGHT / ((dim.width > dim.height) ? dim.width : dim.height);

  if (!cugl::Scene2::init(dim)) {
    return false;
  };

  _map = cugl::scene2::SceneNode::alloc();
  _map->setContentSize(200.0f, 200.0f);
  _map->setPosition(dim / 2);
  _map->setScale(cugl::Vec2::ONE * 2.0f);

  _config.setNumRooms(140);

  cugl::PolyFactory poly_factory;

  std::vector<float> radii{_config.getInnerCircleRadius(),
                           _config.getMiddleCircleRadius(),
                           _config.getMapRadius()};

  for (float r : radii) {
    cugl::Poly2 circle_poly = poly_factory.makeCircle(cugl::Vec2::ZERO, r);
    auto circle = cugl::scene2::PolygonNode::allocWithPoly(circle_poly);
    circle->setAnchor(cugl::Vec2::ANCHOR_CENTER);
    circle->setPosition(dim / 2);
    circle->setScale(cugl::Vec2::ONE * 2.0f);
    circle->setColor(cugl::Color4(0, 0, 0, 20));
    cugl::Scene2::addChild(circle);
  }

  createLevel();

  _map->doLayout();
  cugl::Scene2::addChild(_map);

  // Set color to CORNFLOWER for nostalgia, change in real game.
  cugl::Application::get()->setClearColor(cugl::Color4f::CORNFLOWER);

  return true;
}

void LevelGenerationDemoScene::dispose() {}

void LevelGenerationDemoScene::update(float timestep) {
  if (cugl::Input::get<cugl::Keyboard>()->keyReleased(cugl::KeyCode::R)) {
    _map->removeAllChildren();
    createLevel();
    _map->doLayout();
  }

  std::function<void(void)> nextStep = _level_generator.getNextStep();
  if (nextStep != nullptr) {
    nextStep();
  }
  _ticks++;
}

void LevelGenerationDemoScene::render(
    const std::shared_ptr<cugl::SpriteBatch> &batch) {
  Scene2::render(batch);
}

void LevelGenerationDemoScene::createLevel() {
  _level_generator.dispose();
  _level_generator.init(_config, _map);
  _level_generator.generateRooms();
  _keep_separating_rooms = true;
}