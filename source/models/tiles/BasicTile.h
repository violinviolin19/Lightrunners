#ifndef MODELS_TILES_BASIC_TILE_H_
#define MODELS_TILES_BASIC_TILE_H_

#include <cugl/cugl.h>

class BasicTile : public cugl::scene2::PolygonNode {
 public:
  /**
   * Creates an empty scene graph node with the degenerate texture.
   *
   * This constructor should never be called directly, as this is an abstract
   * class.
   */
  BasicTile() { _classname = "BasicTile"; }
  /**
   * Deletes this node, releasing all resources.
   */
  ~BasicTile() { dispose(); }

  /**
   * Performs a shallow copy of this Node into dst.
   *
   * No children from this node are copied, and no children of dst are
   * modified. In addition, the parents of both Nodes are unchanged. However,
   * all other attributes of this node are copied.
   *
   * @param dst   The Node to copy into
   *
   * @return A reference to dst for chaining.
   */
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

  /** This macro disables the copy constructor (not allowed on scene graphs) */
  CU_DISALLOW_COPY_AND_ASSIGN(BasicTile);
};

#endif  // MODELS_TILES_BASIC_TILE_H_