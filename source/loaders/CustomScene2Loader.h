#ifndef LOADERS_CUSTOM_SCENE_2_LOADER_H_
#define LOADERS_CUSTOM_SCENE_2_LOADER_H_
#include <cugl/assets/CUScene2Loader.h>
#include <cugl/physics2/cu_physics2.h>

#include "../models/tiles/BasicTile.h"
#include "../models/tiles/Terminal.h"

namespace cugl {

/**
 * This class is an extention to the default Scene2Loader. It adds the ability
 * to process tiles and initialize the appropriate classes.
 */
class CustomScene2Loader : public Scene2Loader {
 private:
  /** This macro disables the copy constructor (not allowed on assets) */
  CU_DISALLOW_COPY_AND_ASSIGN(CustomScene2Loader);

 public:
  /**
   * This is an enumeration for identifying tiles types.
   *
   * Each time you add a new tile, it should be added to this list.
   */
  enum class Tile {
    /** A basic tile with no box2d collision. */
    BASIC_TILE,
    /** A wall type with box2d fixed collision. */
    WALL,
    /** A door type that acts as a sensor for leaving rooms. */
    DOOR,
    /** A terminal that acts as a box2d sensor. */
    TERMINAL,
    /** An unsupported type */
    UNKOWN
  };

 protected:
  /** The type map for managing tiles */
  std::unordered_map<std::string, Tile> _tile_types;

  /** A list of all the box2d enabled tiles that were created. */
  std::unordered_map<std::string, std::vector<std::shared_ptr<BasicTile>>>
      _tile_box2d;

  /**
   * Initializes a new asset loader.
   *
   * This method bootstraps the loader with any initial resources that it
   * needs to load assets. Attempts to load an asset before this method is
   * called will fail.
   *
   * This loader will have no associated threads. That means any asynchronous
   * loading will fail until a thread is provided via {@link setThreadPool}.
   *
   * This method is abstract and should be overridden in the specific
   * implementation for each asset.
   *
   * @return true if the asset loader was initialized successfully
   */
  virtual bool init() override { return init(nullptr); }

  /**
   * Initializes a new asset loader.
   *
   * This method bootstraps the loader with any initial resources that it
   * needs to load assets. Attempts to load an asset before this method is
   * called will fail.
   *
   * This method is abstract and should be overridden in the specific
   * implementation for each asset.
   *
   * @param threads   The thread pool for asynchronous loading support
   *
   * @return true if the asset loader was initialized successfully
   */
  virtual bool init(const std::shared_ptr<ThreadPool>& threads) override;

  /**
   * Internal method to support asset loading.
   *
   * This method supports either synchronous or asynchronous loading, as
   * specified by the given parameter.  If the loading is asynchronous,
   * the user may specify an optional callback function.
   *
   * The sources must be a JSON file.  It will parse the JSON tree of this
   * file, assigning the given key to the root node.  The JSON tree should
   * be a tree of widget objects, where each widget object has the following
   * attribute values:
   *
   *      "type":     The node type (a Node or any subclass)
   *      "data":     Data (images, labels) that define the widget.  This
   *                  JSON object has a node-specific format.
   *      "format":   The layout manager to use for this Node. This layout
   *                  manager will apply to all the children (see below).
   *                  This JSON object has a layout-specific format.
   *      "layout":   Node placement using a the layout manager of the parent.
   *                  This is applied after parsing "data' and will override
   *                  any settings there. This JSON object has a
   *                  layout-specific format.
   *      "children": Any child Nodes of this one. This JSON object has a
   *                  named attribute for each child.
   *
   * With the exception of "type", all of these attributes are JSON objects.
   *
   * @param key       The key to access the asset after loading
   * @param source    The pathname to the asset
   * @param callback  An optional callback for asynchronous loading
   * @param async     Whether the asset was loaded asynchronously
   *
   * @return true if the asset was successfully loaded
   */
  virtual bool read(const std::string key, const std::string source,
                    LoaderCallback callback, bool async) override {
    return Scene2Loader::read(key, source, callback, async);
  }

  /**
   * Internal method to support asset loading.
   *
   * This method supports either synchronous or asynchronous loading, as
   * specified by the given parameter.  If the loading is asynchronous,
   * the user may specify an optional callback function.
   *
   * This method is like the traditional read method except that it assumes
   * the JSON data has already been parsed.
   *
   * @param json      The directory entry for the asset
   * @param callback  An optional callback for asynchronous loading
   * @param async     Whether the asset was loaded asynchronously
   *
   * @return true if the asset was successfully loaded
   */
  virtual bool read(const std::shared_ptr<JsonValue>& json,
                    LoaderCallback callback, bool async) override {
    return Scene2Loader::read(json, callback, async);
  }

  /**
   * Unloads the asset for the given directory entry
   *
   * An asset may still be available if it is referenced by a smart pointer.
   * See the description of the specific implementation for how assets
   * are released.
   *
   * This method is abstract and should be overridden in child classes.  You
   * will notice that this method is essentially identical to unload.  We
   * separated the methods because overloading and virtual methods do not
   * place nice.
   *
   * @param json      The directory entry for the asset
   *
   * @return true if the asset was successfully unloaded
   */
  virtual bool purge(const std::shared_ptr<JsonValue>& json) override {
    return Scene2Loader::purge(json);
  }

  /**
   * Attaches all generate nodes to the asset dictionary.
   *
   * As the asset dictionary must be updated in the main thread, we do
   * not update it until the entire node tree has been materialized. This
   * method assumes that each Node is named with its asset look-up key.
   *
   * @param key       The key to access the asset after loading
   * @param node      The scene asset
   *
   * @return true if the node was successfully attached
   */
  virtual bool attach(const std::string& key,
                      const std::shared_ptr<scene2::SceneNode>& node) override;

 public:
#pragma mark -
#pragma mark Constructors
  /**
   * Creates a new, uninitialized scene loader
   *
   * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate a loader on
   * the heap, use one of the static constructors instead.
   */
  CustomScene2Loader() {}

  /**
   * Disposes all resources and assets of this loader
   *
   * Any assets loaded by this object will be immediately released by the
   * loader.  However, a texture may still be available if it is referenced
   * by another smart pointer.  OpenGL will only release a texture asset
   * once all smart pointer attached to the asset are null.
   *
   * Once the loader is disposed, any attempts to load a new asset will
   * fail.  You must reinitialize the loader to begin loading assets again.
   */
  virtual void dispose() override {
    Scene2Loader::dispose();
    _tile_box2d.clear();
    _tile_types.clear();
  }

  /**
   * Returns a newly allocated texture loader.
   *
   * This method bootstraps the loader with any initial resources that it
   * needs to load assets. In particular, the OpenGL context must be active.
   * Attempts to load an asset before this method is called will fail.
   *
   * This loader will have no associated threads. That means any asynchronous
   * loading will fail until a thread is provided via {@link setThreadPool}.
   *
   * @return a newly allocated texture loader.
   */
  static std::shared_ptr<CustomScene2Loader> alloc() {
    std::shared_ptr<CustomScene2Loader> result =
        std::make_shared<CustomScene2Loader>();
    return (result->init() ? result : nullptr);
  }

  /**
   * Returns a newly allocated texture loader.
   *
   * This method bootstraps the loader with any initial resources that it
   * needs to load assets. In particular, the OpenGL context must be active.
   * Attempts to load an asset before this method is called will fail.
   *
   * @param threads   The thread pool for asynchronous loading
   *
   * @return a newly allocated texture loader.
   */
  static std::shared_ptr<CustomScene2Loader> alloc(
      const std::shared_ptr<ThreadPool>& threads) {
    std::shared_ptr<CustomScene2Loader> result =
        std::make_shared<CustomScene2Loader>();
    return (result->init(threads) ? result : nullptr);
  }

  /**
   * Recursively builds the scene from the given JSON tree.
   *
   * This method allows us to maximize the asynchronous creation of scenes.
   * The key is assigned as the name of the root Node of the scene.
   *
   * The JSON tree should be a tree of widget objects, where each widget
   * object has the following attribute values:
   *
   *      "type":     The node type (a Node or any subclass)
   *      "data":     Data (images, labels) that define the widget.  This
   *                  JSON object has a node-specific format.
   *      "format":   The layout manager to use for this Node. This layout
   *                  manager will apply to all the children (see below).
   *                  This JSON object has a layout-specific format.
   *      "layout":   Node placement using a the layout manager of the parent.
   *                  This is applied after parsing "data' and will override
   *                  any settings there. This JSON object has a
   *                  layout-specific format.
   *      "children": Any child Nodes of this one. This JSON object has a
   *                  named attribute for each child.
   *
   * With the exception of "type", all of these attributes are JSON objects.
   *
   * @param key       The key to access the scene after loading
   * @param json      The JSON object defining the scene
   *
   * @return the SDL_Surface with the texture information
   */
  virtual std::shared_ptr<scene2::SceneNode> build(
      const std::string& key,
      const std::shared_ptr<JsonValue>& json) const override;

#pragma mark CustomGetters

  /**
   * @return The list of box2d objects loaded in the scene2 graph.
   */
  std::vector<std::shared_ptr<BasicTile>> getTiles(std::string type) const {
    return _tile_box2d.at(type);
  }
};

}  // namespace cugl

#endif /* LOADERS_CUSTOM_SCENE_2_LOADER_H_ */
