#include "Attack.h"

Attack::Attack() : _currDown(false), _prevDown(false) {}

bool Attack::init() {
  cugl::Mouse *mouse = cugl::Input::get<Mouse>();
  _mouseKey = mouse->acquireKey();
  mouse->addPressListener(_mouseKey, mouseDown);
  mouse->addReleaseListener(_mouseKey, mouseUp);
}

bool Attack::update() {
  _prevDown = _currDown;
  _currDown = _mouseDown;
}

bool Attack::disposse() {
  cugl::Mouse *mouse = cugl::Input::get<Mouse>();
  mouse->removePressListener(_mouseKey);
  mouse->removeReleaseListener(_mouseKey);
}

void Attack::mouseDown(const cugl::MouseEvent &event, Uint8 clicks,
                       bool focus) {
  if (!_mouseDown && event.buttons.hasLeft()) {
    _mouseDown = true;
  }
}

void Attack::mouseUp(const cugl::MouseEvent &event, Uint8 clicks, bool focus) {
  if (_mouseDown && event.buttons.hasLeft()) {
    _mouseDown = false;
  }
}