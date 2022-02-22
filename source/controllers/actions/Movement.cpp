#include "Movement.h"

#define LEFT_ZONE_FRAC 0.5f

#define JOYSTICK_RADIUS 30.0f

Movement::Movement()
    : _show_joystick(false), _joystick_base(nullptr), _joystick(nullptr),
      _listener_key(0) {}

bool Movement::init(const std::shared_ptr<cugl::AssetManager> &assets,
                    cugl::Rect bounds) {
  Action::init(assets, bounds);
  _left_screen_bounds = Action::_display_coord_bounds;
  _left_screen_bounds.size.width *= LEFT_ZONE_FRAC;

  _joystick_base = std::dynamic_pointer_cast<cugl::scene2::PolygonNode>(
      assets->get<cugl::scene2::SceneNode>("ui-scene_joystick-base"));
  _joystick = std::dynamic_pointer_cast<cugl::scene2::PolygonNode>(
      assets->get<cugl::scene2::SceneNode>("ui-scene_joystick"));

#ifdef CU_TOUCH_SCREEN
  cugl::Touchscreen *touch = cugl::Input::get<cugl::Touchscreen>();
  _listener_key = touch->acquireKey();

  touch->addBeginListener(_listener_key,
                          [=](const cugl::TouchEvent &event, bool focus) {
                            this->touchBegan(event, focus);
                          });

  touch->addEndListener(_listener_key,
                        [=](const cugl::TouchEvent &event, bool focus) {
                          this->touchEnded(event, focus);
                        });

  touch->addMotionListener(
      _listener_key,
      [=](const cugl::TouchEvent &event, const cugl::Vec2 &previous,
          bool focus) { this->touchMoved(event, previous, focus); });
#endif // CU_TOUCH_SCREEN

  return true;
}

bool Movement::update() {
  bool update_visibility = (_show_joystick && !_joystick->isVisible() &&
                            !_joystick_base->isVisible()) ||
                           (!_show_joystick && _joystick->isVisible() &&
                            _joystick_base->isVisible());

  if (update_visibility) {
    _joystick->setVisible(_show_joystick);
    _joystick_base->setVisible(_show_joystick);
  }

  _joystick_base->setPosition(_joystick_anchor);
  _joystick->setPosition(_joystick_diff + _joystick_anchor);
  return true;
}

bool Movement::dispose() {
  _joystick = nullptr;
  _joystick_base = nullptr;

#ifdef CU_TOUCH_SCREEN
  cugl::Touchscreen *touch = cugl::Input::get<cugl::Touchscreen>();
  touch->removeBeginListener(_listener_key);
  touch->removeEndListener(_listener_key);
  touch->removeMotionListener(_listener_key);
#endif

  return true;
}

#pragma mark Listeners

#ifdef CU_TOUCH_SCREEN

void Movement::touchBegan(const cugl::TouchEvent &event, bool focus) {
  cugl::Vec2 pos = event.position;

  if (_left_screen_bounds.contains(pos) && _touch_ids.empty()) {
    _touch_ids.insert(event.touch);

    _show_joystick = true;
    _joystick_anchor = Action::displayToScreenCoord(pos);
    _joystick_diff = cugl::Vec2::ZERO;
  }
}

void Movement::touchEnded(const cugl::TouchEvent &event, bool focus) {
  if (_touch_ids.find(event.touch) != _touch_ids.end()) {
    _show_joystick = false;
    _touch_ids.clear();
    _joystick_diff = cugl::Vec2::ZERO;
  }
}

void Movement::touchMoved(const cugl::TouchEvent &event,
                          const cugl::Vec2 &previous, bool focus) {

  if (_touch_ids.find(event.touch) != _touch_ids.end()) {
    cugl::Vec2 pos = event.position;

    _joystick_diff =
        Action::displayToScreenCoord(pos).subtract(_joystick_anchor);

    float clamped_diff_len =
        clampf(_joystick_diff.length(), 0, JOYSTICK_RADIUS);

    _joystick_diff.normalize().scale(clamped_diff_len);
  }
}
#endif // CU_TOUCH_SCREEN
