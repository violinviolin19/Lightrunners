#include "GameScene.h"

#include <cugl/cugl.h>

#include "../controllers/actions/Movement.h"
#include "../controllers/actions/Attack.h"

#define SCENE_HEIGHT 720

bool GameScene::init(const std::shared_ptr<cugl::AssetManager> &assets) {
  // Initialize the scene to a locked width.
  cugl::Size dim = cugl::Application::get()->getDisplaySize();
  dim *= SCENE_HEIGHT / ((dim.width > dim.height) ? dim.width : dim.height);

  if (assets == nullptr || !cugl::Scene2::init(dim)) {
    return false;
  }
  _assets = assets;

  auto world_layer = assets->get<cugl::scene2::SceneNode>("world-scene");
  world_layer->setContentSize(dim);
  world_layer->doLayout();
  cugl::Scene2::addChild(world_layer);

  auto ui_layer = assets->get<cugl::scene2::SceneNode>("ui-scene");
  ui_layer->setContentSize(dim);
  ui_layer->doLayout();
  cugl::Scene2::addChild(ui_layer);

  InputController::get()->init(_assets, cugl::Scene2::getBounds());

  // Set color to BLACK.
  cugl::Application::get()->setClearColor(cugl::Color4f::BLACK);

  // Create the world and attach the listeners.
  _world = cugl::physics2::ObstacleWorld::alloc(
      cugl::Rect(0, 0, dim.width, dim.height), cugl::Vec2(0, 0));

  _world_node = cugl::scene2::SceneNode::alloc();
  cugl::Scene2::addChild(_world_node);

    // Debug world
    _debug_node = cugl::scene2::SceneNode::alloc();
    cugl::Scene2::addChild(_debug_node);
    
  populate(dim);

  return true;
}

void GameScene::dispose() { InputController::get()->dispose(); }

void GameScene::populate(cugl::Size dim) {
  // Initialize the player with texture and size, then add to world.
  std::shared_ptr<cugl::Texture> player = _assets->get<cugl::Texture>("player");

  _player = Player::alloc(dim/2, cugl::Size(96, 96), "Johnathan");

  auto player_n = cugl::scene2::SpriteNode::alloc(player, 3, 10);
  _player->setPlayerNode(player_n);
  _world_node->addChild(player_n);
  _world->addObstacle(_player);
    _world->addObstacle(_player->getSword());

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
  // Movement
  std::shared_ptr<Movement> mvm = InputController::get<Movement>();
    _player->move(mvm->getMovementX(), mvm->getMovementY());
    std::shared_ptr<Attack> att =InputController::get<Attack>();
    _player->attack(att->isAttacking());
    _grunt->move(-.5, 0);
  _world->update(timestep);

  // Animation
  _player->animate(mvm->getMovementX(), mvm->getMovementY());
}

void GameScene::render(const std::shared_ptr<cugl::SpriteBatch> &batch) {
  Scene2::render(batch);
}
