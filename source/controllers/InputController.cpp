#include "InputController.h"
#include "actions/Action.h"
#include "actions/Attack.h"
#include "actions/Movement.h"
#include <cugl/cugl.h>

// static
std::shared_ptr<InputController> InputController::_singleton = nullptr;

InputController::InputController() : _active(false) {}

bool InputController::init(const std::shared_ptr<cugl::AssetManager> &assets,
                           cugl::Rect bounds) {

#ifdef CU_TOUCH_SCREEN
  cugl::Touchscreen *input = cugl::Input::get<cugl::Touchscreen>();
#else
  cugl::Mouse *input = cugl::Input::get<cugl::Mouse>();
#endif

  if (input) {
    // Create, initialize and register all the actions.
    _active = true;
    std::shared_ptr<Attack> attack = std::make_shared<Attack>();
    _active = attack->init(assets, bounds);
    _actions[std::type_index(typeid(Attack))] =
        std::dynamic_pointer_cast<Action>(attack);

    std::shared_ptr<Movement> movement = std::make_shared<Movement>();
    _active = movement->init(assets, bounds);
    _actions[std::type_index(typeid(Movement))] =
        std::dynamic_pointer_cast<Action>(movement);
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
  _actions.clear();
  return successful;
}
