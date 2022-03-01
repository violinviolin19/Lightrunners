#include "Room.h"

#pragma mark Room

Room::Room(cugl::Size size)
    : _type(Room::RoomType::DEFAULT), _visited(false), _fixed(false) {
  _node = cugl::scene2::PolygonNode::alloc();
  _node->setContentSize(size);

  for (float x = 0.0f; x < size.width; x++) {
    for (float y = 0.0f; y < size.height; y++) {
      cugl::Rect tile_rect(cugl::Vec2::ZERO, cugl::Vec2::ONE);
      auto tile = cugl::scene2::PathNode::allocWithRect(tile_rect, 0.1f);
      // tile->setContentSize(cugl::Vec2::ONE * 0.8f);
      tile->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);
      tile->setPosition(floorf(x) + 0.1f, floorf(y) + 0.1f);
      tile->setColor(cugl::Color4::BLACK);
      _node->addChild(tile);
    }
  }

  _node->doLayout();
}

void Room::move(cugl::Vec2 dir) {
  if (!_fixed) {
    dir.set(roundf(dir.x), roundf(dir.y));
    _node->setPosition(_node->getPosition() + dir);
  }
}

bool Room::hasNeighbor(const std::shared_ptr<Room> &room) const {
  return findEdge(room) != nullptr;
}

std::shared_ptr<Edge> Room::findEdge(const std::shared_ptr<Room> &room) const {
  auto result = std::find_if(
      _edges.begin(), _edges.end(), [room](const std::shared_ptr<Edge> &edge) {
        return edge->_source == room || edge->_neighbor == room;
      });

  return (result != _edges.end()) ? *result : nullptr;
}

#pragma mark Edge

Edge::Edge(const std::shared_ptr<Room> &s, const std::shared_ptr<Room> &n)
    : _weight(0), _active(true) {
  _source = s;
  _neighbor = n;
  std::vector<cugl::Vec2> vertices{_source->getMid(), _neighbor->getMid()};
  _path = cugl::scene2::PathNode::allocWithVertices(vertices, 0.4f);
  _path->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);
  _path->setPosition(_path->getBoundingRect().origin);
  _path->setColor(cugl::Color4(0, 0, 0, 50));
  _weight = (vertices[1] - vertices[0]).length();
}

bool Edge::doesIntersect(cugl::Vec2 origin, float r) {

  cugl::Vec2 d = _neighbor->getMid() - _source->getMid();
  cugl::Vec2 f = _source->getMid() - origin;

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
