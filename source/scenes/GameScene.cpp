#include "GameScene.h"

#include <cugl/cugl.h>

#include "../controllers/actions/Movement.h"
#include "../loaders/CustomScene2Loader.h"
#include "../models/tiles/Wall.h"

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

  _debug_node = cugl::scene2::SceneNode::alloc();
  _debug_node->setContentSize(dim);

  // Create the world and attach the listeners.
  cugl::Rect bounds = cugl::Rect(cugl::Vec2::ZERO, dim);
  _world = cugl::physics2::ObstacleWorld::alloc(bounds);
  populate(dim);

  _world_node->doLayout();

  auto ui_layer = assets->get<cugl::scene2::SceneNode>("ui-scene");
  ui_layer->setContentSize(dim);
  ui_layer->doLayout();

  cugl::Scene2::addChild(_world_node);
  cugl::Scene2::addChild(ui_layer);
  cugl::Scene2::addChild(_debug_node);
  _debug_node->setVisible(false);

  InputController::get()->init(_assets, cugl::Scene2::getBounds());

  // Set color to BLACK.
  cugl::Application::get()->setClearColor(cugl::Color4f::BLACK);

  return true;
}

void GameScene::dispose() { InputController::get()->dispose(); }

void GameScene::populate(cugl::Size dim) {
  // Initialize the player with texture and size, then add to world.
  std::shared_ptr<cugl::Texture> player = _assets->get<cugl::Texture>("player");
  cugl::Size player_size(player->getSize());

  _player = Player::alloc(dim / 2.0f, player_size, "Johnathan");

  auto player_node = cugl::scene2::PolygonNode::allocWithTexture(player);
  _player->setPlayerNode(player_node);
  _player->setDebugColor(cugl::Color4::RED);
  _player->setDebugScene(_debug_node);
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

  // Add physics enabled tiles to world node, debug node and box2d physics
  // world.
  std::shared_ptr<cugl::CustomScene2Loader> loader =
      std::dynamic_pointer_cast<cugl::CustomScene2Loader>(
          _assets->access<cugl::scene2::SceneNode>());

  for (std::shared_ptr<BasicTile> tile : loader->getTiles("wall")) {
    auto wall = std::dynamic_pointer_cast<Wall>(tile);
    _world->addObstacle(wall->initBox2d());
    wall->getObstacle()->setDebugColor(cugl::Color4::GREEN);
    wall->getObstacle()->setDebugScene(_debug_node);
  }
}

void GameScene::update(float timestep) {
  InputController::get()->update();

  _player->move(InputController::get<Movement>()->getMovement());

  _grunt->move(-.5, 0);
  _world->update(timestep);
}

void GameScene::render(const std::shared_ptr<cugl::SpriteBatch> &batch) {
  Scene2::render(batch);
}
