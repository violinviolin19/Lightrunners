#ifndef CONTROLLERS_CONTROLLER_H_
#define CONTROLLERS_CONTROLLER_H_

/**
 * Parent class for all controllers. init() methods should be done in the
 * controller, as initializations are very personal.
 */
class Controller {
 public:
  /** Construct a new controller. */
  Controller() {}
  /** Destroy the controller. */
  ~Controller() { dispose(); }

  /** Update the controller state. */
  virtual void update() {}

  /** Dispose the controller and all its values. */
  virtual void dispose() {}
};

#endif  // CONTROLLERS_CONTROLLER_H_