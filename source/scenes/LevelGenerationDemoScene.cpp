#include "LevelGenerationDemoScene.h"

#include <cugl/cugl.h>

#include "../generators/LevelGeneratorConfig.h"

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
  _map->setScale(cugl::Vec2::ONE * 5.0f);

  // _config.setNumRooms(20);

  cugl::PolyFactory poly_factory;

  std::vector<float> radii{
      _config.getInnerCircleRadius(),
      _config.getMiddleCircleRadius() + _config.getSeparationBetweenLayers(),
      _config.getMapRadius() + 2 * _config.getSeparationBetweenLayers()};

  for (float r : radii) {
    cugl::Poly2 circle_poly = poly_factory.makeCircle(cugl::Vec2::ZERO, r);
    auto circle = cugl::scene2::PolygonNode::allocWithPoly(circle_poly);
    circle->setAnchor(cugl::Vec2::ANCHOR_CENTER);
    circle->setPosition(dim / 2);
    circle->setScale(cugl::Vec2::ONE * 1.5f);
    circle->setColor(cugl::Color4(0, 0, 0, 20));
    cugl::Scene2::addChild(circle);
  }

  createLevel();

  _map->doLayout();
  cugl::Scene2::addChild(_map);

  cugl::Application::get()->setClearColor(cugl::Color4f::WHITE);

  return true;
}

void LevelGenerationDemoScene::dispose() {}

void LevelGenerationDemoScene::update(float timestep) {
  cugl::Keyboard *keyboard = cugl::Input::get<cugl::Keyboard>();

  if (keyboard->keyReleased(cugl::KeyCode::R)) {
    _map->removeAllChildren();
    createLevel();
    _map->doLayout();
  }

  cugl::Vec2 scale = _map->getScale();
  if (keyboard->keyDown(cugl::KeyCode::I))
    scale *= 1.05f;
  else if (keyboard->keyDown(cugl::KeyCode::O))
    scale *= 0.95f;
  _map->setScale(scale);

  cugl::Vec2 pos = _map->getPosition();
  if (keyboard->keyDown(cugl::KeyCode::W))
    pos.y -= 5.0f;
  else if (keyboard->keyDown(cugl::KeyCode::S))
    pos.y += 5.0f;

  if (keyboard->keyDown(cugl::KeyCode::D))
    pos.x -= 5.0f;
  else if (keyboard->keyDown(cugl::KeyCode::A))
    pos.x += 5.0f;

  _map->setPosition(pos);

  _level_generator.update();

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