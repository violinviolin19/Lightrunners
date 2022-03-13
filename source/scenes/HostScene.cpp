#include "HostScene.h"

#include <cugl/cugl.h>

#include <iostream>
#include <sstream>

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT 720

#pragma mark -
#pragma mark Host Methods

bool HostScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
  // Initialize the scene to a locked width
  cugl::Size dimen = cugl::Application::get()->getDisplaySize();
  dimen *= SCENE_HEIGHT / dimen.height;
  if (assets == nullptr) {
    return false;
  } else if (!Scene2::init(dimen)) {
    return false;
  }

  // Start up the input handler
  _assets = assets;

  // Acquire the scene built by the asset loader and resize it the scene
  std::shared_ptr<cugl::scene2::SceneNode> scene =
      _assets->get<cugl::scene2::SceneNode>("host");
  scene->setContentSize(dimen);
  scene->doLayout();  // Repositions the HUD

  _startgame = std::dynamic_pointer_cast<cugl::scene2::Button>(
      _assets->get<cugl::scene2::SceneNode>("host_center_start"));
  _backout = std::dynamic_pointer_cast<cugl::scene2::Button>(
      _assets->get<cugl::scene2::SceneNode>("host_back"));
  _gameid = std::dynamic_pointer_cast<cugl::scene2::Label>(
      _assets->get<cugl::scene2::SceneNode>("host_center_game_field_text"));
  _player = std::dynamic_pointer_cast<cugl::scene2::Label>(
      _assets->get<cugl::scene2::SceneNode>("host_center_players_field_text"));
  _status = Status::WAIT;

  // Program the buttons
  _backout->addListener([this](const std::string& name, bool down) {
    if (down) {
      disconnect();
      _status = Status::ABORT;
    }
  });

  _startgame->addListener([this](const std::string& name, bool down) {
    if (down) {
      startGame();
    }
  });

  // Create the server configuration
  auto json = _assets->get<cugl::JsonValue>("server");
  _config.punchthroughServerAddr = json->getString("address", "");
  _config.punchthroughServerPort = json->getInt("port", 0);
  _config.maxNumPlayers = json->getInt("maximum", 0);
  _config.apiVersion = json->getInt("version", 0);

  addChild(scene);
  setActive(false);
  return true;
}

void HostScene::dispose() {
  if (_active) {
    removeAllChildren();
    _active = false;
  }
}

void HostScene::setActive(bool value) {
  if (isActive() != value) {
    Scene2::setActive(value);
    if (value) {
      _status = WAIT;
      configureStartButton();
      _backout->activate();
      connect();
    } else {
      _startgame->deactivate();
      _backout->deactivate();
      // If any were pressed, reset them
      _startgame->setDown(false);
      _backout->setDown(false);
    }
  }
}

void HostScene::updateText(const std::shared_ptr<cugl::scene2::Button>& button,
                           const std::string text) {
  auto label = std::dynamic_pointer_cast<cugl::scene2::Label>(
      button->getChildByName("up")->getChildByName("label"));
  label->setText(text);
}

void HostScene::update(float timestep) {
  if (_network) {
    _network->receive(
        [this](const std::vector<uint8_t>& data) { processData(data); });
    checkConnection();
    // Do this last for button safety
    configureStartButton();
  }
}

void HostScene::processData(const std::vector<uint8_t>& data) {
  // TODO process data as needed
}

bool HostScene::connect() {
  _network = cugl::NetworkConnection::alloc(_config);
  return checkConnection();
}

bool HostScene::checkConnection() {
  switch(_network->getStatus()) {
      case cugl::NetworkConnection::NetStatus::Pending:
          _status = WAIT;
          break;
      case cugl::NetworkConnection::NetStatus::Connected:
          _gameid->setText(_network->getRoomID());
          _player->setText(std::to_string(_network->getNumPlayers()));
          if (_status != START) {
              _status = IDLE;
          }
          break;
      case cugl::NetworkConnection::NetStatus::Reconnecting:
          _status = WAIT;
          break;
      case cugl::NetworkConnection::NetStatus::RoomNotFound:
      case cugl::NetworkConnection::NetStatus::ApiMismatch:
      case cugl::NetworkConnection::NetStatus::GenericError:
      case cugl::NetworkConnection::NetStatus::Disconnected:
          _status = WAIT;
          return false;
  }
  return true;
}

void HostScene::configureStartButton() {
  if (_status == Status::WAIT) {
    _startgame->deactivate();
    updateText(_startgame, "Waiting");
  } else {
    updateText(_startgame, "Start Game");
    _startgame->activate();
  }
}

void HostScene::startGame() {
  std::vector<uint8_t> msg = { 255 };
  _network->send(msg);
  _status = START;
}
