#include "Attack.h"

Attack::Attack() : _currDown(false), _prevDown(false) {}

bool Attack::init() {
  cugl::Mouse *mouse = cugl::Input::get<cugl::Mouse>();
  _mouseKey = mouse->acquireKey();

  mouse->addPressListener(
      _mouseKey, [=](const cugl::MouseEvent &event, Uint8 clicks, bool focus) {
        if (!_mouseDown && event.buttons.hasLeft()) {
          _mouseDown = true;
        }
      });
  mouse->addReleaseListener(
      _mouseKey, [=](const cugl::MouseEvent &event, Uint8 clicks, bool focus) {
        if (_mouseDown && event.buttons.hasLeft()) {
          _mouseDown = false;
        }
      });
  return true;
}

bool Attack::update() {
  _prevDown = _currDown;
  _currDown = _mouseDown;
  return true;
}

bool Attack::dispose() {
  cugl::Mouse *mouse = cugl::Input::get<cugl::Mouse>();
  mouse->removePressListener(_mouseKey);
  mouse->removeReleaseListener(_mouseKey);
  return true;
}