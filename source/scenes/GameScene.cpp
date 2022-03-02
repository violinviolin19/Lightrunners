#include "GameScene.h"

#include <cugl/cugl.h>

#include "../controllers/actions/Movement.h"

#define SCENE_HEIGHT 720

bool GameScene::init(const std::shared_ptr<cugl::AssetManager> &assets) {
  // Initialize the scene to a locked width.
  cugl::Size dim = cugl::Application::get()->getDisplaySize();
  dim *= SCENE_HEIGHT / ((dim.width > dim.height) ? dim.width : dim.height);

  if (assets == nullptr || !cugl::Scene2::init(dim)) {
    return false;
  }
  _assets = assets;

  _world_node = assets->get<cugl::scene2::SceneNode>("world-scene");
  _world_node->setContentSize(dim);

  // Create the world and attach the listeners.
  cugl::Rect bounds = cugl::Rect(cugl::Vec2::ZERO, dim);
  _world = cugl::physics2::ObstacleWorld::alloc(bounds);
  populate(dim);

  _world_node->doLayout();
  cugl::Scene2::addChild(_world_node);

  auto ui_layer = assets->get<cugl::scene2::SceneNode>("ui-scene");
  ui_layer->setContentSize(dim);
  ui_layer->doLayout();
  cugl::Scene2::addChild(ui_layer);

  InputController::get()->init(_assets, cugl::Scene2::getBounds());

  // Set color to BLACK.
  cugl::Application::get()->setClearColor(cugl::Color4f::BLACK);

  return true;
}

void GameScene::dispose() { InputController::get()->dispose(); }

void GameScene::populate(cugl::Size dim) {
  // Initialize the player with texture and size, then add to world.
  std::shared_ptr<cugl::Texture> player = _assets->get<cugl::Texture>("player");
  cugl::Size playerSize(player->getSize());

  _player = Player::alloc(dim / 2.0f, playerSize, "Johnathan");

  auto player_node = cugl::scene2::PolygonNode::allocWithTexture(player);
  _player->setPlayerNode(player_node);
  _world_node->addChild(player_node);
  _world->addObstacle(_player);

  // Initialize the grunt with texture and size, then add to world.
  std::shared_ptr<cugl::Texture> grunt = _assets->get<cugl::Texture>("grunt");
  cugl::Size gruntSize(grunt->getSize());

  _grunt = Grunt::alloc(dim / 2.3f, gruntSize, "Grunt");

  auto grunt_node = cugl::scene2::SpriteNode::alloc(grunt, 1, 1);
  _grunt->setGruntNode(grunt_node);
  _world_node->addChild(grunt_node);
  _world->addObstacle(_grunt);
}

void GameScene::update(float timestep) {
  InputController::get()->update();

  _player->move(InputController::get<Movement>()->getMovement());

  _world_node->setPosition(_player->getPosition());

  _grunt->move(-.5, 0);
  _world->update(timestep);
}

void GameScene::render(const std::shared_ptr<cugl::SpriteBatch> &batch) {
  Scene2::render(batch);
}
