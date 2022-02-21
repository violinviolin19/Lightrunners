#ifndef CONTROLLERS_ACTIONS_ACTION_H_
#define CONTROLLERS_ACTIONS_ACTION_H_
#include <cugl/cugl.h>

class Action {
protected:
  /* Action has been initialized correctly. */
  bool _action_init = false;

  /* Bounds of the current display, used to scale input */
  cugl::Rect _display_coord_bounds;

  /* Bounds of the scene2 world, used to scale input */
  cugl::Rect _screen_coord_bounds;

public:
  Action() {}
  ~Action() {}

  /**
   * Creates input listeners and sets default variable.
   * @param assets The loaded assets for this game mode.
   * @param bounds The scene2 game bounds.
   * @return If initialized correctly.
   */
  virtual bool init(const std::shared_ptr<cugl::AssetManager> &assets,
                    cugl::Rect bounds) {
    _screen_coord_bounds = bounds;
    _display_coord_bounds = cugl::Application::get()->getDisplayBounds();
    _action_init = true;
    return _action_init;
  }

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

  /**
   * Returns the scene location of a touch
   *
   * Touch coordinates are inverted, with y origin in the top-left
   * corner. This method corrects for this and scales the screen
   * coordinates down on to the scene graph size.
   *
   * @return the scene location of a touch
   */
  cugl::Vec2 displayToScreenCoord(const cugl::Vec2 pos) const {
    cugl::Vec2 result;

    if (_action_init) {
      float px = pos.x / _display_coord_bounds.size.width -
                 _display_coord_bounds.origin.x;
      float py = pos.y / _display_coord_bounds.size.height -
                 _display_coord_bounds.origin.y;
      result.x =
          px * _screen_coord_bounds.size.width + _screen_coord_bounds.origin.x;
      result.y = (1 - py) * _screen_coord_bounds.size.height +
                 _screen_coord_bounds.origin.y;
    }
    return result;
  }
};

#endif /* CONTROLLERS_ACTIONS_ACTION_H_ */
