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
  _level_loading.dispose();
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
  switch (_current_scene) {
    case LOADING:
      if (_loading.isActive()) {
        _loading.update(0.01f);
        break;
      }

      _menu.init(_assets);
      _current_scene = Scene::MENU;
      _loading.dispose();  // Disables the input listeners in this mode.
      break;

    case MENU:
      if (_menu.getChoice() == MenuScene::Choice::NONE) {
        _menu.update(timestep);
        break;
      }

      _level_loading.init(_assets);
      _current_scene = Scene::LEVEL_LOADING;
      _menu.dispose();
      break;

    case LEVEL_LOADING:
      if (_level_loading.isActive()) {
        _level_loading.update(timestep);
        break;
      }

      _gameplay.init(_assets, _level_loading.getLevelGenerator());
      _current_scene = Scene::GAME;
      _level_loading.dispose();
      break;

    case GAME:
      _gameplay.update(timestep);
      break;

    default:
      break;
  }
}

void GameApp::draw() {
  switch (_current_scene) {
    case LOADING:
      _loading.render(_batch);
      break;

    case MENU:
      _menu.render(_batch);
      break;

    case LEVEL_LOADING:
      _level_loading.render(_batch);
      break;

    case GAME:
      _gameplay.render(_batch);
      break;

    default:
      break;
  }
}
