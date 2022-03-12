#include "MenuScene.h"

#include <cugl/cugl.h>

#include <iostream>
#include <sstream>

#pragma mark -
#pragma mark Level Layout

/** Regardless of logo, lock the height to this */
#define SCENE_HEIGHT 720

#pragma mark -
#pragma mark Constructors

bool MenuScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
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
      _assets->get<cugl::scene2::SceneNode>("menu");
  scene->setContentSize(dimen);
  scene->doLayout();  // Repositions the HUD
  _choice = Choice::NONE;
  _hostbutton = std::dynamic_pointer_cast<cugl::scene2::Button>(
      _assets->get<cugl::scene2::SceneNode>("menu_host"));
  _joinbutton = std::dynamic_pointer_cast<cugl::scene2::Button>(
      _assets->get<cugl::scene2::SceneNode>("menu_join"));

  // Program the buttons
  _hostbutton->addListener([this](const std::string& name, bool down) {
    if (down) {
      _choice = Choice::HOST;
    }
  });
  _joinbutton->addListener([this](const std::string& name, bool down) {
    if (down) {
      _choice = Choice::JOIN;
    }
  });

  addChild(scene);
  setActive(false);
  return true;
}

void MenuScene::dispose() {
  if (_active) {
    removeAllChildren();
    _active = false;
  }
}

void MenuScene::setActive(bool value) {
  if (isActive() != value) {
    Scene2::setActive(value);
    if (value) {
      _choice = NONE;
      _hostbutton->activate();
      _joinbutton->activate();
    } else {
      _hostbutton->deactivate();
      _joinbutton->deactivate();
      // If any were pressed, reset them
      _hostbutton->setDown(false);
      _joinbutton->setDown(false);
    }
  }
}