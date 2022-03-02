#ifndef LOADERS_TILE_LOADER_H_
#define LOADERS_TILE_LOADER_H_
#include <cugl/cugl.h>

#include "../models/tiles/BasicTile.h"
#include "../models/tiles/Wall.h"

class cugl::AssetManager;

/**
 * This class is a specific implementation of Loader<BasicTile>
 *
 * This asset loader allows us to spawn a box2d node, which can
 * be attached to a new or existing scene graph.
 *
 * As with all of our loaders, this loader is designed to be attached to an
 * asset manager. Use the method {@link getHook()} to get the appropriate
 * pointer for attaching the loader.
 */
class TileLoader : public cugl::Loader<BasicTile> {
 private:
  /** This macro disables the copy constructor (not allowed on assets) */
  CU_DISALLOW_COPY_AND_ASSIGN(Box2dLoader);

 protected:
  /**
   * This is an enumeration for identifying scene node types.
   *
   * Each time you add a new UI widget, it should be added to this list.
   */
  enum class Tile {
    /** The base Node type */
    NODE,
    /** A wall tile */
    WALL,
    /** A floor tile */
    FLOOR
  };

  /** The type map for managing constructors */
  std::unordered_map<std::string, Tile> _types;

#pragma mark Asset Loading
  /**
   * Finishes loading the sound file, setting its default volume.
   *
   * Allocating a sound asset can be done safely in a separate thread.
   * However, setting the default volume requires the audio engine, and so
   * this step is not safe to be done in a separate thread.  Instead, it
   * takes place in the main CUGL thread via {@link Application#schedule}.
   *
   * This method supports an optional callback function which reports whether
   * the asset was successfully materialized.
   *
   * @param key       The key to access the asset after loading
   * @param sound     The sound asset partially loaded
   * @param callback  An optional callback for asynchronous loading
   */
  void materialize(const std::string& key,
                   const std::shared_ptr<BasicTile>& sound,
                   LoaderCallback callback);

  /**
   * Internal method to support asset loading.
   *
   * This method supports either synchronous or asynchronous loading, as
   * specified by the given parameter.  If the loading is asynchronous,
   * the user may specify an optional callback function.
   *
   * This method will split the loading across the Sound allocation and
   * the internal {@link materialize} method.  This ensures that asynchronous
   * loading is safe.
   *
   * @param key       The key to access the asset after loading
   * @param source    The pathname to the asset
   * @param callback  An optional callback for asynchronous loading
   * @param async     Whether the asset was loaded asynchronously
   *
   * @return true if the asset was successfully loaded
   */
  virtual bool read(const std::string key, const std::string source,
                    cugl::LoaderCallback callback, bool async) override;

  /**
   * Internal method to support asset loading.
   *
   * This method supports either synchronous or asynchronous loading, as
   * specified by the given parameter.  If the loading is asynchronous,
   * the user may specify an optional callback function.
   *
   * This method will split the loading across the Sound allocation and
   * the internal {@link materialize} method.  This ensures that asynchronous
   * loading is safe.
   *
   * This version of read provides support for JSON directories. A soundfx
   * directory entry has the following values
   *
   *      "file":         The path to the asset
   *      "volume":       This default sound volume (float)
   *
   * @param json      The directory entry for the asset
   * @param callback  An optional callback for asynchronous loading
   * @param async     Whether the asset was loaded asynchronously
   *
   * @return true if the asset was successfully loaded
   */
  virtual bool read(const std::shared_ptr<JsonValue>& json,
                    cugl::Accelerometer callback, bool async) override;

 public:
#pragma mark -
#pragma mark Constructors
  /**
   * Creates a new, uninitialized tile loader
   *
   * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate a loader on
   * the heap, use one of the static constructors instead.
   */
  TileLoader() {}
  ~TileLoader() { dispose(); }

  /**
   * Disposes all resources and assets of this loader
   *
   * Any assets loaded by this object will be immediately released by the
   * loader.  However, a sound may still be available if it is referenced
   * by another smart pointer.  The audio engine will only release a sound
   * buffer once all smart pointer attached to the asset are null.
   *
   * Once the loader is disposed, any attempts to load a new asset will
   * fail.  You must reinitialize the loader to begin loading assets again.
   */
  void dispose() override {
    _assets.clear();
    _loader = nullptr;
  }

  /**
   * Returns a newly allocated tile loader.
   *
   * This method bootstraps the loader with any initial resources that it
   * needs to load tiles.
   *
   * This loader will have no associated threads. That means any asynchronous
   * loading will fail until a thread is provided via {@link setThreadPool}.
   *
   * @return a newly allocated asset loader.
   */
  static std::shared_ptr<TileLoader> alloc() {
    std::shared_ptr<TileLoader> result = std::make_shared<TileLoader>();
    return (result->init() ? result : nullptr);
  }

  /**
   * Returns a newly allocated tile loader.
   *
   * This method bootstraps the loader with any initial resources that it
   * needs to load assets. In particular, the OpenGL context must be active.
   * Attempts to load an asset before this method is called will fail.
   *
   * @param threads   The thread pool for asynchronous loading
   *
   * @return a newly allocated tile loader.
   */
  static std::shared_ptr<TileLoader> alloc(
      const std::shared_ptr<cugl::ThreadPool>& threads) {
    std::shared_ptr<TileLoader> result = std::make_shared<TileLoader>();
    return (result->init(threads) ? result : nullptr);
  }
};

#endif /* LOADERS_TILE_LOADER_H_ */
