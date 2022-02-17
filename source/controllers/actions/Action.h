#ifndef CONTROLLERS_ACTIONS_ACTION_H_
#define CONTROLLERS_ACTIONS_ACTION_H_
#include <cugl/cugl.h>

class Action {
protected:
  /** The name of this action. */
  std::string _name;

public:
  Action(){};

  virtual bool init();

  virtual bool update();

  virtual bool dispose();

  std::string getName() { return _name; }
};

#endif /* CONTROLLERS_ACTIONS_ACTION_H_ */