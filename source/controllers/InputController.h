#ifndef CONTROLLERS_INPUT_CONTROLLER_H_
#define CONTROLLERS_INPUT_CONTROLLER_H_
#include "actions/Action.h"
#include <cugl/cugl.h>

class InputController {
protected:
  bool _active;
  static std::shared_ptr<InputController> _singleton;

  std::unordered_map<std::type_index, std::shared_ptr<Action>> _actions;

public:
  static std::shared_ptr<InputController> get() {
    if (_singleton == nullptr) {
      _singleton = std::make_shared<InputController>();
    }
    return singleton;
  }

  template <typename T> static std::shared_ptr<T> get() {
    if (!_singleton) {
      return nullptr;
    }
    std::type_index indx = std::type_index(typeid(T));
    auto it = _singleton->_actions.find(indx);
    if (it != _singleton->_actions.end()) {
      return static_cast<T *>(it->second);
    }
    return nullptr;
  }

  bool init();

  bool dispose();

  bool update();

  bool isActive() { return _active; }

  InputController(InputController const &) = delete;
  void operator=(InputController const &) = delete;
  ~InputController() { dispose(); }

private:
  InputController();
};

#endif /* CONTROLLERS_INPUT_CONTROLLER_H_ */