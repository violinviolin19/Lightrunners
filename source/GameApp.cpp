#include "GameApp.h"

#include "loaders/CustomScene2Loader.h"

void GameApp::onStartup() {
  _assets = cugl::AssetManager::alloc();
  _batch = cugl::SpriteBatch::alloc();
  auto cam = cugl::OrthographicCamera::alloc(getDisplaySize());

#ifdef CU_TOUCH_SCREEN
  cugl::Input::activate<cugl::Touchscreen>();
#else
  cugl::Input::activate<cugl::Mouse>();
  // cugl::Mouse does not track mouse drag or move by default.
  cugl::Input::get<cugl::Mouse>()->setPointerAwareness(
      cugl::Mouse::PointerAwareness::ALWAYS);
#endif
  cugl::Input::activate<cugl::Keyboard>();
  cugl::Input::activate<cugl::TextInput>();

  // Add asset loaders.
  _assets->attach<cugl::Texture>(cugl::TextureLoader::alloc()->getHook());
  _assets->attach<cugl::Font>(cugl::FontLoader::alloc()->getHook());
  _assets->attach<cugl::JsonValue>(cugl::JsonLoader::alloc()->getHook());
  _assets->attach<cugl::WidgetValue>(cugl::WidgetLoader::alloc()->getHook());
  _assets->attach<cugl::scene2::SceneNode>(
      cugl::CustomScene2Loader::alloc()->getHook());

  // Create a "loading" screen.
  _loaded = false;
  _loading.init(_assets);

  // Queue up the other assets (EMPTY in this case).
  _assets->loadDirectoryAsync("json/assets.json", nullptr);

  cugl::Application::onStartup();  // YOU MUST END with call to parent.
}

void GameApp::onShutdown() {
  _loading.dispose();
  _gameplay.dispose();
  _hostgame.dispose();
  _joingame.dispose();
  _menu.dispose();
#ifndef CU_TOUCH_SCREEN
  _level_gen_scene.dispose();
#endif
  _assets = nullptr;
  _batch = nullptr;

#ifdef CU_TOUCH_SCREEN
  cugl::Input::deactivate<cugl::Touchscreen>();
#else
  cugl::Input::deactivate<cugl::Mouse>();
#endif

  cugl::Application::onShutdown();  // YOU MUST END with call to parent.
}

void GameApp::update(float timestep) {
#ifndef CU_TOUCH_SCREEN
  if (cugl::Input::get<cugl::Keyboard>()->keyReleased(cugl::KeyCode::L)) {
    _show_level_gen_scene = !_show_level_gen_scene;
  }
#endif

  switch (_scene) {
    case LOAD:
      updateLoadingScene(timestep);
      break;
    case MENU:
      updateMenuScene(timestep);
      break;
    case HOST:
      updateHostMenuScene(timestep);
      break;
    case CLIENT:
      updateClientMenuScene(timestep);
      break;
    case GAME:
      updateGameScene(timestep);
      break;
  }
}

void GameApp::draw() {
  switch (_scene) {
    case LOAD:
      _loading.render(_batch);
      break;
    case MENU:
      _menu.render(_batch);
      break;
    case HOST:
      _hostgame.render(_batch);
      break;
    case CLIENT:
      _joingame.render(_batch);
      break;
    case GAME:
      //    case GAME_HOST:
      _gameplay.render(_batch);
      break;
  }
}

/**
 * Individualized update method for the loading scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the loading scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameApp::updateLoadingScene(float timestep) {
  if (!_loaded && _loading.isActive()) {
    _loading.update(timestep);
  } else if (!_loaded) {
    _loading
        .dispose();  // Permanently disables the input listeners in this mode.
    _menu.init(_assets);
    _hostgame.init(_assets);
    _joingame.init(_assets);
    _gameplay.init(_assets);
#ifndef CU_TOUCH_SCREEN
    _level_gen_scene.init();
#endif
    _menu.setActive(true);
    _hostgame.setActive(false);
    _joingame.setActive(false);
    _gameplay.setActive(false);
    _scene = State::MENU;
    _loaded = true;
  }
}

/**
 * Individualized update method for the menu scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the menu scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameApp::updateMenuScene(float timestep) {
  _menu.update(timestep);
  switch (_menu.getChoice()) {
    case MenuScene::Choice::HOST:
      _menu.setActive(false);
      _hostgame.setActive(true);
      _scene = State::HOST;
      break;
    case MenuScene::Choice::JOIN:
      _menu.setActive(false);
      _joingame.setActive(true);
      _scene = State::CLIENT;
      break;
    case MenuScene::Choice::NONE:
      // DO NOTHING
      break;
  }
}

/**
 * Individualized update method for the host scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the host scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameApp::updateHostMenuScene(float timestep) {
  _hostgame.update(timestep);
  switch (_hostgame.getStatus()) {
    case HostMenuScene::Status::ABORT:
      _hostgame.setActive(false);
      _menu.setActive(true);
      _scene = State::MENU;
      break;
    case HostMenuScene::Status::START:
      _hostgame.setActive(false);
      _gameplay.setActive(true);
      _scene = State::GAME;
      // Transfer connection ownership
      _gameplay.setConnection(_hostgame.getConnection());
      _hostgame.disconnect();
      _gameplay.setHost(true);
      break;
    case HostMenuScene::Status::WAIT:
    case HostMenuScene::Status::IDLE:
    case HostMenuScene::Status::JOIN:
      // DO NOTHING
      break;
  }
}

/**
 * Individualized update method for the client scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the client scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameApp::updateClientMenuScene(float timestep) {
  _joingame.update(timestep);
  switch (_joingame.getStatus()) {
    case ClientMenuScene::Status::ABORT:
      _joingame.setActive(false);
      _menu.setActive(true);
      _scene = State::MENU;
      break;
    case ClientMenuScene::Status::START:
      _joingame.setActive(false);
      _menu.setActive(false);
      _gameplay.setActive(true);
      _scene = State::GAME;
      // Transfer connection ownership
      _gameplay.setConnection(_joingame.getConnection());
      _joingame.disconnect();
      _gameplay.setHost(false);
      break;
    case ClientMenuScene::Status::WAIT:
    case ClientMenuScene::Status::IDLE:
    case ClientMenuScene::Status::JOIN:
      // DO NOTHING
      break;
  }
}

/**
 * Individualized update method for the game scene.
 *
 * This method keeps the primary {@link #update} from being a mess of switch
 * statements. It also handles the transition logic from the game scene.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameApp::updateGameScene(float timestep) { _gameplay.update(timestep); }

///**
// * Individualized update method for the game scene.
// *
// * This method keeps the primary {@link #update} from being a mess of switch
// * statements. It also handles the transition logic from the game scene.
// *
// * @param timestep  The amount of time (in seconds) since the last frame
// */
// void GameApp::updateHostGameScene(float timestep) {
//    _gameplay.update(timestep);
//}
