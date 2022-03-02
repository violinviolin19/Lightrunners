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

  populate(dim);

  return true;
}

void GameScene::dispose() { InputController::get()->dispose(); }

void GameScene::populate(cugl::Size dim) {
  // The player
  std::shared_ptr<cugl::Texture> player = _assets->get<cugl::Texture>("player");
  std::shared_ptr<cugl::Texture> player_move = _assets->get<cugl::Texture>("player-move");
  cugl::Size playerSize(player->getSize());

  _player = Player::alloc(cugl::Vec2(dim.width / 2, dim.height / 2), playerSize, "Johnathan");
    _player->setIdleTexture(player);
    _player->setMovingTexture(player_move);

  auto player_n = cugl::scene2::SpriteNode::alloc(player, 1, 1);
  _player->setPlayerNode(player_n);
  _world_node->addChild(player_n);
  _world->addObstacle(_player);
}

void GameScene::update(float timestep) {
  InputController::get()->update();
    // Movement
    std::shared_ptr<Movement> mvm = InputController::get<Movement>();
    _player->move(mvm->getMovementX(), mvm->getMovementY());
    _world->update(timestep);
    
    // Animation
    _player->animate(mvm->getMovementX(), mvm->getMovementY());
}

void GameScene::render(const std::shared_ptr<cugl::SpriteBatch> &batch) {
  Scene2::render(batch);
}
