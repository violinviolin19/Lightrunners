#include "InputController.h"
#include "actions/Action.h"
#include "actions/Attack.h"
#include <cugl/cugl.h>

// static
std::shared_ptr<InputController> InputController::_singleton = nullptr;

InputController::InputController() : _active(false) {}

bool InputController::init() {
  cugl::Mouse *mouse = cugl::Input::get<cugl::Mouse>();
  if (mouse) {
    _active = true;
    std::shared_ptr<Attack> attack = std::make_shared<Attack>();
    _active = attack->init();
    _actions[std::type_index(typeid(Attack))] =
        std::dynamic_pointer_cast<Action>(attack);
  }
  return _active;
}

bool InputController::update() {
  for (auto it = _actions.begin(); it != _actions.end(); ++it) {
    (it->second)->update();
  }
  return true;
}

bool InputController::dispose() {
  bool successful = false;
  if (_active) {
    successful = true;
    for (auto it = _actions.begin(); it != _actions.end(); ++it) {
      successful = (it->second)->dispose();
    }
    _active = false;
  }
  return successful;
}
