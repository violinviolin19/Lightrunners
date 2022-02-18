#ifndef CONTROLLERS_ACTIONS_ATTACK_H_
#define CONTROLLERS_ACTIONS_ATTACK_H_
#include "Action.h"
#include <cugl/cugl.h>

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
};

#endif /* CONTROLLERS_ACTIONS_ATTACK_H_ */
