#ifndef MODELS_TILES_BASIC_TILE_H_
#define MODELS_TILES_BASIC_TILE_H_

#include <cugl/cugl.h>

class BasicTile : public cugl::scene2::TexturedNode {
 public:
  /**
   * Creates an empty scene graph node with the degenerate texture.
   *
   * This constructor should never be called directly, as this is an abstract
   * class.
   */
  BasicTile() : TexturedNode() { _classname = "BasicTile"; }
  /**
   * Deletes this node, releasing all resources.
   */
  ~BasicTile() { dispose(); }

  /**
   * Disposes all of the resources used by this node.
   *
   * A disposed Node can be safely reinitialized. Any children owned by this
   * node will be released.  They will be deleted if no other object owns them.
   *
   * It is unsafe to call this on a Node that is still currently inside of
   * a scene graph.
   */
  virtual void dispose() override { TexturedNode::dispose(); }

  bool init() override { return TexturedNode::init(); }

  bool initWithFile(const std::string& filename) override {
    return TexturedNode::initWithFile(filename);
  }

  bool initWithTexture(const std::shared_ptr<cugl::Texture>& texture) override {
    return TexturedNode::initWithTexture(texture);
  }

  virtual std::shared_ptr<SceneNode> copy(
      const std::shared_ptr<SceneNode>& dst) const override {
    return TexturedNode::copy(dst);
  }

  /**
   * Initializes a tile node with the given JSON specificaton.
   *
   * This initializer is designed to receive the "data" object from the
   * JSON passed to {@link Scene2Loader}. This JSON format supports all
   * of the attribute values of its parent class.
   *
   * @param loader    The scene loader passing this JSON file
   * @param data      The JSON object specifying the node
   *
   * @return true if initialization was successful.
   */
  virtual bool initWithData(
      const cugl::Scene2Loader* loader,
      const std::shared_ptr<cugl::JsonValue>& data) override;

  /**
   * Returns a new tile node with the given JSON specificaton.
   *
   * This initializer is designed to receive the "data" object from the
   * JSON passed to {@link Scene2Loader}. This JSON format supports all
   * of the attribute values of its parent class.
   *
   * @param loader    The scene loader passing this JSON file
   * @param data      The JSON object specifying the node
   *
   * @return a new polygon with the given JSON specificaton.
   */
  static std::shared_ptr<SceneNode> allocWithData(
      const cugl::Scene2Loader* loader,
      const std::shared_ptr<cugl::JsonValue>& data) {
    std::shared_ptr<BasicTile> result = std::make_shared<BasicTile>();
    if (!result->initWithData(loader, data)) {
      result = nullptr;
    }
    return std::dynamic_pointer_cast<SceneNode>(result);
  }

  /**
   * Returns a string representation of this node for debugging purposes.
   *
   * If verbose is true, the string will include class information.  This
   * allows us to unambiguously identify the class.
   *
   * @param verbose Whether to include class information
   *
   * @return a string representation of this node for debuggging purposes.
   */
  std::string toString(bool verbose = false) const override {
    return TexturedNode::toString(verbose);
  }

  /**
   * Sets the untransformed size of the node.
   *
   * The content size remains the same no matter how the node is scaled or
   * rotated. All nodes must have a size, though it may be degenerate (0,0).
   *
   * By default, the content size of a TexturedNode is the size of the
   * bounding box of the defining polygon. Resizing a texture node will
   * stretch the image to fill in the new size.
   *
   * @param size  The untransformed size of the node.
   */
  void setContentSize(const cugl::Size size) override {
    return TexturedNode::setContentSize(size);
  }
};

#endif  // MODELS_TILES_BASIC_TILE_H_