#include "GameApp.h"

#include "loaders/CustomScene2Loader.h"

void GameApp::onStartup() {
  _assets = cugl::AssetManager::alloc();
  _batch = cugl::SpriteBatch::alloc();
  auto cam = cugl::OrthographicCamera::alloc(getDisplaySize());

#ifdef CU_TOUCH_SCREEN
  cugl::Input::activate<cugl::Touchscreen>();
#else
  cugl::Input::activate<cugl::Keyboard>();
  cugl::Input::activate<cugl::Mouse>();
  cugl::Input::activate<cugl::Keyboard>();
  // cugl::Mouse does not track mouse drag or move by default.
  cugl::Input::get<cugl::Mouse>()->setPointerAwareness(
      cugl::Mouse::PointerAwareness::ALWAYS);
#endif

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

  if (!_loaded && _loading.isActive()) {
    _loading.update(0.01f);
  } else if (!_loaded) {
    _loading.dispose();  // Disables the input listeners in this mode.
    _menu.init(_assets);
    _gameplay.init(_assets);
    _menu.setActive(true);
    _gameplay.setActive(false);
#ifndef CU_TOUCH_SCREEN
    _level_gen_scene.init();
#endif
    _loaded = true;
#ifndef CU_TOUCH_SCREEN
  } else if (_show_level_gen_scene) {
    _level_gen_scene.update(timestep);
#endif
  } else {
    if (_menu.getChoice() == MenuScene::Choice::HOST ||
        _menu.getChoice() == MenuScene::Choice::JOIN) {
      _gameplay.setActive(true);
      _menu.setActive(false);
      _gameplay.update(timestep);
    } else {
      _menu.update(timestep);
    }
  }
}

void GameApp::draw() {
  if (!_loaded) {
    _loading.render(_batch);
#ifndef CU_TOUCH_SCREEN
  } else if (_show_level_gen_scene) {
    _level_gen_scene.render(_batch);
#endif
  } else {
    if (_menu.getChoice() == MenuScene::Choice::HOST ||
        _menu.getChoice() == MenuScene::Choice::JOIN) {
      _gameplay.render(_batch);
    } else {
      _menu.render(_batch);
    }
  }
}
