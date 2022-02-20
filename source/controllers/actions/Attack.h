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
  /**
   * Creates input listeners and sets default variable.
   * @return If initialized correctly.
   */
  virtual bool init() override;

  /**
   * Updates action state.
   * @return If updated correctly.
   */
  virtual bool update() override;

  /**
   * Disposes input listeners and default variables.
   * @return If disposed correctly.
   */
  virtual bool dispose() override;

  /**
   * @return If player is currently attacking.
   */
  bool isAttacking() const { return !_prevDown && _currDown; }

  Attack();
  ~Attack() {}
};

#endif /* CONTROLLERS_ACTIONS_ATTACK_H_ */
