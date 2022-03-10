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
  // TODO
  return true;
}

void HostScene::dispose() {
  if (_active) {
    removeAllChildren();
    _active = false;
  }
}

void HostScene::setActive(bool value) {
  // TODO
}

void HostScene::updateText(const std::shared_ptr<cugl::scene2::Button>& button,
                           const std::string text) {
  // TODO
}

void HostScene::update(float timestep) {
  // TODO
}

void HostScene::processData(const std::vector<uint8_t>& data) {
  // TODO
}

bool HostScene::connect() {
  // TODO
  return true;
}

bool HostScene::checkConnection() {
  // TODO
  return true;
}

void HostScene::configureStartButton() {
  // TODO
}

void HostScene::startGame() {
  // TODO
}
