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

  virtual bool init() = 0;

  virtual bool update() = 0;

  virtual bool dispose() = 0;

  std::string getName() { return _name; }
};

#endif /* CONTROLLERS_ACTIONS_ACTION_H_ */
