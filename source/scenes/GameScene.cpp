#include "GameScene.h"

#include "../controllers/actions/Movement.h"
#include "../Player.hpp"
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

  // Create the world and attach the listeners.
  _world = physics2::ObstacleWorld::alloc(Rect(0, 0, dim.width, dim.height), Vec2(0, 0));
//  _world->activateCollisionCallbacks(true);
  
  _worldnode = scene2::SceneNode::alloc();
  cugl::Scene2::addChild(_worldnode);
    
  populate(dim);
    
  return true;
}

void GameScene::dispose() { InputController::get()->dispose(); }

void GameScene::populate(Size dim) {
    // The player
    std::shared_ptr<cugl::Texture> player = _assets->get<cugl::Texture>("player");
    cugl::Size playerSize(player->getSize());
    
    _player = Player::alloc(cugl::Vec2(dim.width/2, dim.height/2), playerSize, "Johnathan");
    
    auto playerNode = scene2::PolygonNode::allocWithTexture(player);
    _player->setPlayerNode(playerNode);
    _worldnode->addChild(playerNode);
    _world->addObstacle(_player);
}

void GameScene::update(float timestep) { InputController::get()->update();
    std::shared_ptr<Movement> mvm = InputController::get<Movement>();
    _player->move(mvm->getMovementX(), mvm->getMovementY());
    
    _world->update(timestep);
}

void GameScene::render(const std::shared_ptr<cugl::SpriteBatch> &batch) {
  Scene2::render(batch);
}
