#include "CustomScene2Loader.h"

#include "../models/tiles/BasicTile.h"
#include "../models/tiles/Wall.h"

namespace cugl {

/** If the type is unknown */
#define UNKNOWN_STR "<unknown>"

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
bool CustomScene2Loader::init(const std::shared_ptr<ThreadPool>& threads) {
  if (!Scene2Loader::init(threads)) return false;

  _tile_types["basictile"] = Tile::BASIC_TILE;
  _tile_types["wall"] = Tile::WALL;

  return true;
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
std::shared_ptr<scene2::SceneNode> CustomScene2Loader::build(
    const std::string& key, const std::shared_ptr<JsonValue>& json) const {
  bool nonrelative = false;
  std::shared_ptr<JsonValue> data = json->get("data");
  std::shared_ptr<scene2::SceneNode> node = nullptr;
  std::string type = json->getString("type", UNKNOWN_STR);
  auto it = _types.find(strtool::tolower(type));
  if (it != _types.end()) {
    switch (it->second) {
      case Widget::NODE:
        node = scene2::SceneNode::allocWithData(this, data);
        break;
      case Widget::IMAGE:
        node = scene2::PolygonNode::allocWithData(this, data);
        break;
      case Widget::SOLID:
        // TODO: Replace with polygon as first child and sized to fill
        // That will keep us from breaking the tint abstraction
        node = scene2::PolygonNode::allocWithData(this, data);
        nonrelative = true;
        break;
      case Widget::ORDER:
          node = scene2::OrderedNode::allocWithData(this,data);
          break;
      case Widget::CANVAS:
          node = scene2::CanvasNode::allocWithData(this,data);
          break;
      case Widget::POLY:
        node = scene2::PolygonNode::allocWithData(this, data);
        break;
      case Widget::PATH:
        node = scene2::PathNode::allocWithData(this, data);
        break;
      case Widget::WIRE:
        node = scene2::WireNode::allocWithData(this, data);
        break;
      case Widget::ANIMATE:
        node = scene2::SpriteNode::allocWithData(this, data);
        break;
      case Widget::NINE:
        node = scene2::NinePatch::allocWithData(this, data);
        break;
      case Widget::LABEL:
        node = scene2::Label::allocWithData(this, data);
        break;
      case Widget::BUTTON:
        node = scene2::Button::allocWithData(this, data);
        break;
      case Widget::PROGRESS:
        node = scene2::ProgressBar::allocWithData(this, data);
        break;
      case Widget::SLIDER:
        node = scene2::Slider::allocWithData(this, data);
        break;
      case Widget::SCROLL:
        node = scene2::ScrollPane::allocWithData(this, data);
        break;
      case Widget::TEXTFIELD:
        node = scene2::TextField::allocWithData(this, data);
        break;
      case Widget::EXTERNAL_IMPORT: {
        const std::shared_ptr<JsonValue> widgetJson = getWidgetJson(json);
        return build(key, widgetJson);
      }
      case Widget::UNKNOWN:
        break;
    }
  }

  auto kt = _tile_types.find(strtool::tolower(type));
  if (kt != _tile_types.end()) {
    switch (kt->second) {
      case Tile::BASIC_TILE:
        node = BasicTile::allocWithData(this, data);
        break;
      case Tile::WALL:
        node = Wall::allocWithData(this, data);
        break;
      case Tile::UNKOWN:
        break;
    }
  }

  if (node == nullptr) {
    return nullptr;
  }

  if (node->getContentSize() == Size::ZERO) {
    node->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    node->setContentSize(Display::get()->getBounds().size);
  }

  std::shared_ptr<JsonValue> form = json->get("format");
  std::string ftype =
      (form == nullptr ? UNKNOWN_STR : form->getString("type", UNKNOWN_STR));
  auto jt = _forms.find(strtool::tolower(ftype));

  std::shared_ptr<scene2::Layout> layout = nullptr;
  if (jt != _forms.end()) {
    switch (jt->second) {
      case Form::ANCHORED:
        layout = scene2::AnchoredLayout::allocWithData(form);
        break;
      case Form::FLOAT:
        layout = scene2::FloatLayout::allocWithData(form);
        break;
      case Form::GRID:
        layout = scene2::GridLayout::allocWithData(form);
        break;
      case Form::NONE:
      case Form::UNKNOWN:
        break;
    }
  }
  node->setLayout(layout);

  std::shared_ptr<JsonValue> children = json->get("children");
  if (children != nullptr) {
    for (int ii = 0; ii < children->size(); ii++) {
      std::shared_ptr<JsonValue> item = children->get(ii);
      std::string key = item->key();
      if (key != "comment") {
        // If this is a widget, use the loaded widget json instead
        if (item->has("type") && item->getString("type") == "Widget") {
          item = getWidgetJson(item);
        }

        std::shared_ptr<scene2::SceneNode> kid = build(key, item);
        if (nonrelative) {
          kid->setRelativeColor(false);
        }
        node->addChild(kid);

        if (layout != nullptr && item->has("layout")) {
          std::shared_ptr<JsonValue> posit = item->get("layout");
          layout->add(key, posit);
        }
      }
    }
  }

  // Do not perform layout yet.
  node->setName(key);
  return node;
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
bool CustomScene2Loader::attach(
    const std::string& key, const std::shared_ptr<scene2::SceneNode>& node) {
  _assets[key] = node;

  // Check if class is a tile.
  auto it = _tile_types.find(strtool::tolower(node->getClassName()));
  if (it != _tile_types.end()) {
    _tile_box2d[strtool::tolower(node->getClassName())].push_back(
        std::dynamic_pointer_cast<BasicTile>(node));
  }

  bool success = true;
  for (int ii = 0; ii < node->getChildren().size(); ii++) {
    std::shared_ptr<scene2::SceneNode> item = node->getChild(ii);
    std::string local = key + "_" + item->getName();
    success = attach(local, item) && success;
  }
  return success;
}

}  // namespace cugl
