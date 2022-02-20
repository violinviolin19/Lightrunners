#include "GameApp.h"

void GameApp::onStartup() {
  _assets = cugl::AssetManager::alloc();
  _batch = cugl::SpriteBatch::alloc();
  auto cam = cugl::OrthographicCamera::alloc(getDisplaySize());

#ifdef CU_TOUCH_SCREEN
  cugl::Input::activate<cugl::Touchscreen>();
#else
  cugl::Input::activate<cugl::Mouse>();
#endif

  // Add asset loaders.
  _assets->attach<cugl::Texture>(cugl::TextureLoader::alloc()->getHook());
  _assets->attach<cugl::scene2::SceneNode>(
      cugl::Scene2Loader::alloc()->getHook());

  // Create a "loading" screen.
  _loaded = false;
  _loading.init(_assets);

  // Queue up the other assets (EMPTY in this case).
  _assets->loadDirectoryAsync("json/assets.json", nullptr);

  cugl::Application::onStartup(); // YOU MUST END with call to parent.
}

void GameApp::onShutdown() {
  _loading.dispose();
  _gameplay.dispose();
  _assets = nullptr;
  _batch = nullptr;

#ifdef CU_TOUCH_SCREEN
  cugl::Input::deactivate<cugl::Touchscreen>();
#else
  cugl::Input::deactivate<cugl::Mouse>();
#endif

  cugl::Application::onShutdown(); // YOU MUST END with call to parent.
}

void GameApp::update(float timestep) {
  if (!_loaded && _loading.isActive()) {
    _loading.update(0.01f);
  } else if (!_loaded) {
    _loading.dispose(); // Disables the input listeners in this mode.
    _gameplay.init(_assets);
    _loaded = true;
  } else {
    _gameplay.update(timestep);
  }
}

void GameApp::draw() {
  if (!_loaded) {
    _loading.render(_batch);
  } else {
    _gameplay.render(_batch);
  }
}
