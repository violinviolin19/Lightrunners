#include "Room.h"

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

Room::Room(cugl::Size size)
    : type(Room::RoomType::DEFAULT), visited(false), fixed(false) {
  node = cugl::scene2::PolygonNode::alloc();
  node->setContentSize(size);

  for (float i = 0.0f; i <= size.width * size.height; i++) {
    auto tile = cugl::scene2::PolygonNode::alloc();
    tile->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);
    tile->setPosition(y = floorf(i / size.width), fmod(i, size.width));
    node->addChild()
  }
}

void Room::move(cugl::Vec2 dir) {
  if (!fixed) {
    dir.set(roundf(dir.x), roundf(dir.y));
    node->setPosition(node->getPosition() + dir);
  }
}

bool Room::hasNeighbor(const std::shared_ptr<Room> room) const {
  auto result = std::find_if(
      edges.begin(), edges.end(), [room](const std::shared_ptr<Edge> &edge) {
        return edge->source == room || edge->neighbor == room;
      });

  return result != edges.end();
}

std::shared_ptr<Edge> Room::findEdge(const std::shared_ptr<Room> room) const {
  auto result = std::find_if(
      edges.begin(), edges.end(), [room](const std::shared_ptr<Edge> &edge) {
        return edge->source == room || edge->neighbor == room;
      });

  return (result != edges.end()) ? *result : nullptr;
}