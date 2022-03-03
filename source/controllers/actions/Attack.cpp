#include "Attack.h"

Attack::Attack() : _curr_down(false), _prev_down(false), _button(nullptr) {}

bool Attack::init(const std::shared_ptr<cugl::AssetManager> &assets,
                  cugl::Rect bounds) {
  _button = std::dynamic_pointer_cast<cugl::scene2::Button>(
      assets->get<cugl::scene2::SceneNode>("ui-scene_attack"));

  _button->addListener(
      [=](const std::string &name, bool down) { _butt_down = down; });

  _button->activate();

  return true;
}

bool Attack::update() {
  _prev_down = _curr_down;
  _curr_down = _butt_down;
  return true;
}

bool Attack::dispose() {
  _button = nullptr;
  return true;
}

void Attack::setActive(bool value) {
  (value) ? _button->activate() : _button->deactivate();
}
