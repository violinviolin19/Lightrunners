#include "Hallway.h"

Hallway::Hallway(std::shared_ptr<Edge> edge, cugl::Poly2 poly) : _edge(edge) {
  _node = cugl::scene2::PolygonNode::allocWithPoly(poly);
  _node->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);
  _node->setPosition(_node->getBoundingRect().origin);
  _node->setColor(cugl::Color4(0, 0, 0, 100));
}