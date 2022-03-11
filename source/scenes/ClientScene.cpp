#include "ClientScene.h"

#include <cugl/cugl.h>

#include <iostream>
#include <sstream>

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT 720

#pragma mark -
#pragma mark Client Methods

bool ClientScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
  // TODO factor out a lot of this common code to peer scene (if we keep the
  // scene stuff)

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
      _assets->get<cugl::scene2::SceneNode>("client");
  scene->setContentSize(dimen);
  scene->doLayout();  // Repositions the HUD

  _startgame = std::dynamic_pointer_cast<cugl::scene2::Button>(
      _assets->get<cugl::scene2::SceneNode>("client_center_start"));
  _backout = std::dynamic_pointer_cast<cugl::scene2::Button>(
      _assets->get<cugl::scene2::SceneNode>("client_back"));
  _gameid = std::dynamic_pointer_cast<cugl::scene2::TextField>(
      _assets->get<cugl::scene2::SceneNode>("client_center_game_field_text"));
  _player = std::dynamic_pointer_cast<cugl::scene2::Label>(
      _assets->get<cugl::scene2::SceneNode>(
          "client_center_players_field_text"));
  _status = Status::IDLE;

  _backout->addListener([this](const std::string& name, bool down) {
    if (down) {
      disconnect();
      _status = Status::ABORT;
    }
  });

  _startgame->addListener([=](const std::string& name, bool down) {
    if (down) {
      // This will call the _gameid listener
      if (_gameid->hasFocus()) {
        _gameid->releaseFocus();
      } else {
        connect(_gameid->getText());
      }
    }
  });

  _gameid->addExitListener(
      [this](const std::string& name, const std::string& value) {
        connect(value);
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

void ClientScene::dispose() {
  if (_active) {
    removeAllChildren();
    _active = false;
  }
}

void ClientScene::setActive(bool value) {
  if (isActive() != value) {
    Scene2::setActive(value);
    if (value) {
      _status = IDLE;
      _gameid->activate();
      _backout->activate();
      _network = nullptr;
      _player->setText("1");
      configureStartButton();
      // Don't reset the room id
    } else {
      _gameid->deactivate();
      _startgame->deactivate();
      _backout->deactivate();
      // If any were pressed, reset them
      _startgame->setDown(false);
      _backout->setDown(false);
    }
  }
}

void ClientScene::updateText(
    const std::shared_ptr<cugl::scene2::Button>& button,
    const std::string text) {
  auto label = std::dynamic_pointer_cast<cugl::scene2::Label>(
      button->getChildByName("up")->getChildByName("label"));
  label->setText(text);
}

void ClientScene::update(float timestep) {
  if (_network) {
    _network->receive(
        [this](const std::vector<uint8_t>& data) { processData(data); });
    checkConnection();
    // Do this last for button safety
    configureStartButton();
  }
}

bool ClientScene::connect(const std::string room) {
  _network = cugl::NetworkConnection::alloc(_config, room);
  return checkConnection();
}

void ClientScene::processData(const std::vector<uint8_t>& data) {
  if (data[0] == 255) {
    _status = Status::START;
  }
}

bool ClientScene::checkConnection() {
  cugl::NetworkConnection::NetStatus status = _network->getStatus();
  if (status == cugl::NetworkConnection::NetStatus::Pending) {
    _status = Status::JOIN;
  } else if (status == cugl::NetworkConnection::NetStatus::Connected) {
    if (_status != Status::START) {
      _status = Status::WAIT;
    }
  } else if (status != cugl::NetworkConnection::NetStatus::Reconnecting) {
    _network->dispose();
    _status = Status::IDLE;
    return false;
  }

  _player->setText(std::to_string(_network->getNumPlayers()));

  return true;
}

void ClientScene::configureStartButton() {
  if (_status == Status::IDLE) {
    _startgame->activate();
    updateText(_startgame, "Start Game");
  } else if (_status == Status::JOIN) {
    _startgame->deactivate();
    updateText(_startgame, "Connecting");
  } else if (_status == Status::WAIT) {
    updateText(_startgame, "Waiting");
  }
}
