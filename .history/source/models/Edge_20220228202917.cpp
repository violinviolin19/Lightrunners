#include "Edge.h"

Edge::Edge(const std::shared_ptr<Room> &s, const std::shared_ptr<Room> &n)
    : weight(0), active(true) {
  source = s;
  neighbor = n;
  std::vector<cugl::Vec2> vertices{source->getMid(), neighbor->getMid()};
  path = cugl::scene2::PathNode::allocWithVertices(vertices, 0.4f);
  path->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);
  path->setPosition(path->getBoundingRect().origin);
  path->setColor(cugl::Color4(0, 0, 0, 50));
  weight = (vertices[1] - vertices[0]).length();
}

bool Edge::doesIntersect(cugl::Vec2 origin, float r) {

  cugl::Vec2 d = neighbor->getMid() - source->getMid();
  cugl::Vec2 f = source->getMid() - origin;

  float a = d.dot(d);
  float b = 2 * f.dot(d);
  float c = f.dot(f) - r * r;

  float discriminant2 = b * b - 4 * a * c;
  if (discriminant2 < 0) {
    return false;
  }

  float discriminant = sqrt(discriminant2);

  float t1 = (-1 * b - discriminant) / (2 * a);
  float t2 = (-1 * b + discriminant) / (2 * a);

  return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
}
