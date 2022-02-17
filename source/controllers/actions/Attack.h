#ifndef CONTROLLERS_ACTIONS_ATTACK_H_
#define CONTROLLERS_ACTIONS_ATTACK_H_
#include <cugl/cugl.h>

// Forward declaration of Action to prevent circular dependencies.
class Action;

class Attack : public Action {
protected:
  bool _mouseDown;
  bool _currDown;
  bool _prevDown;
  Uint32 _mouseKey;

public:
  virtual bool init() override;
  virtual bool update() override;
  virtual bool dispose() override;

  bool isAttacking() const { return !_prevDown && _currDown; }

  Attack();
  ~Attack() {}

protected:
  void mouseDown(const cugl::MouseEvent &event, Uint8 clicks, bool focus);
  void mouseUp(const cugl::MouseEvent &event, Uint8 clicks, bool focus);
};

#endif /* CONTROLLERS_ACTIONS_ATTACK_H_ */