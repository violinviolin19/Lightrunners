#ifndef CONTROLLERS_ACTIONS_ACTION_H_
#define CONTROLLERS_ACTIONS_ACTION_H_
#include <cugl/cugl.h>

class Action {
protected:
  /** The name of this action. */
  std::string _name;

public:
  Action() {}
  ~Action() {}

  /**
   * Creates input listeners and sets default variable.
   * @return If initialized correctly.
   */
  virtual bool init() = 0;

  /**
   * Updates action state.
   * @return If updated correctly.
   */
  virtual bool update() = 0;

  /**
   * Disposes input listeners and default variables.
   * @return If disposed correctly.
   */
  virtual bool dispose() = 0;

  std::string getName() { return _name; }
};

#endif /* CONTROLLERS_ACTIONS_ACTION_H_ */
