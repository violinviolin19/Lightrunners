#include "Room.h"

#pragma mark Room

Room::Room(cugl::Size size)
    : _type(Room::RoomType::STANDARD), _visited(false), _fixed(false) {
  // If the doors haven't been filled before, then fill it automatically.
  if (_doors.size() == 0) {
    // Set the doors to the middle of the four edges in the room.
    _doors.push_back(cugl::Vec2(floorf(size.width / 2.0f), 0.0f));
    _doors.push_back(cugl::Vec2(floorf(size.width / 2.0f), size.height - 1.0f));
    _doors.push_back(cugl::Vec2(0.0f, floorf(size.height / 2.0f)));
    _doors.push_back(cugl::Vec2(size.width - 1, floorf(size.height / 2.0f)));
  }

  initScene2(size);
}

Room::Room(cugl::Size size, std::vector<cugl::Vec2> doors)
    : _type(Room::RoomType::STANDARD), _visited(false), _fixed(false) {
  for (cugl::Vec2 &door : doors) {
    // If the door is within the bounds of the edges of the room then add it to
    // the rooms doors.
    if ((door.x == 0.0f || door.x == size.width - 1) &&
        (door.y > 0.0f && door.y < size.height - 1)) {
      _doors.push_back(door);
    }

    if ((door.y == 0.0f || door.y == size.height - 1) &&
        (door.x > 0.0f && door.x < size.width - 1)) {
      _doors.push_back(door);
    }
  }

  initScene2(size);
}

void Room::move(cugl::Vec2 dist) {
  if (!_fixed) {
    dist.set(roundf(dist.x), roundf(dist.y));
    _node->setPosition(_node->getPosition() + dist);
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

void Room::initScene2(cugl::Size size) {
  _node = cugl::scene2::PolygonNode::alloc();
  _node->setContentSize(size);
  // Fill the tiles inside of the room.
  for (float x = 0.0f; x < size.width; x++) {
    for (float y = 0.0f; y < size.height; y++) {
      cugl::Rect tile_rect(cugl::Vec2::ZERO, cugl::Vec2::ONE);
      auto tile = cugl::scene2::PathNode::allocWithRect(tile_rect, 0.1f);
      // tile->setContentSize(cugl::Vec2::ONE * 0.8f);
      tile->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);
      cugl::Vec2 pos(floorf(x), floorf(y));
      tile->setPosition(pos + cugl::Vec2(0.1f, 0.1f));

      // If the tile is a door.
      if (std::find(_doors.begin(), _doors.end(), pos) != _doors.end()) {
        tile->setColor(cugl::Color4::ORANGE);
      } else {
        tile->setColor(cugl::Color4::BLACK);
      }

      _node->addChild(tile);
    }
  }

  _node->doLayout();
}

#pragma mark Edge

Edge::Edge(const std::shared_ptr<Room> &s, const std::shared_ptr<Room> &n)
    : _weight(0), _calculated(false) {
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
