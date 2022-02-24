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
    // Create, initialize and register all basic actions.
    _active = true;

    _active = InputController::attachAction<Attack>(
        Attack::alloc(assets, bounds)->getHook());

    _active = InputController::attachAction<Movement>(
        Movement::alloc(assets, bounds)->getHook());
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