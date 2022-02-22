#ifndef CONTROLLERS_ACTIONS_ATTACK_H_
#define CONTROLLERS_ACTIONS_ATTACK_H_
#include "Action.h"
#include <cugl/cugl.h>

/**
 * This class is an implementation of Action.
 *
 * This class provides attacking capabilities for the user.
 *
 * As with all Actions attach to InputController by calling allocating using
 * alloc and calling getHook(). This is very similar to Walker White's loader
 * system.
 */
class Attack : public Action {
protected:
  /* Reference to attack button for registering listeners to press event. */
  std::shared_ptr<cugl::scene2::Button> _button;

  /* Button was previously down on the last tick. */
  bool _prev_down;
  /* Button is currently down on the current tick. */
  bool _curr_down;
  /* Scene2 button is pressed. */
  bool _butt_down;

public:
  /**
   * Creates input listeners and sets default variable.
   * @param assets The loaded assets for this game mode.
   * @param bounds The scene2 game bounds.
   * @return If initialized correctly.
   */
  virtual bool init(const std::shared_ptr<cugl::AssetManager> &assets,
                    cugl::Rect bounds) override;

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
   * This method allocates Attack and initializes it.
   *
   * @param assets The loaded assets for this game mode.
   * @param bounds The scene2 game bounds.
   * @return A newly allocated Attack action.
   */
  static std::shared_ptr<Attack>
  alloc(const std::shared_ptr<cugl::AssetManager> &assets, cugl::Rect bounds) {

    std::shared_ptr<Attack> result = std::make_shared<Attack>();
    return (result->init(assets, bounds) ? result : nullptr);
  }

  /**
   * @return If player is currently attacking.
   */
  bool isAttacking() const { return !_prev_down && _curr_down; }

  /**
   * Toggels activation on attack button. When deactivated, the button cannot
   * be pressed.
   * @param value The activation state.
   */
  void setActive(bool value);

  Attack();
  ~Attack() {}
};

#endif /* CONTROLLERS_ACTIONS_ATTACK_H_ */
