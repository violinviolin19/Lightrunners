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
  // TODO
  return true;
}

void ClientScene::dispose() {
  if (_active) {
    removeAllChildren();
    _active = false;
  }
}

void ClientScene::setActive(bool value) {
  // TODO
}

void ClientScene::updateText(const std::shared_ptr<cugl::scene2::Button>& button, const std::string text) {
// TODO
}

void ClientScene::update(float timestep) {
  // TODO
}

bool ClientScene::connect(const std::string room) {
  // TODO
  return true;
}

void ClientScene::processData(const std::vector<uint8_t>& data) {
  // TODO
}

bool ClientScene::checkConnection() {
  // TODO

  return true;
}

void ClientScene::configureStartButton() {
  // TODO
}
