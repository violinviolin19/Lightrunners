#ifndef MODELS_ROOM_H
#define MODELS_ROOM_H
#include <cugl/cugl.h>

class Edge;

class Room {
 public:
  enum RoomType { TERMINAL, SPAWN, DEFAULT };

  RoomType _type;

  std::shared_ptr<cugl::scene2::PolygonNode> _node;

  std::vector<std::shared_ptr<Edge>> _edges;

  bool _visited;
  bool _fixed;

  Room(float width, float height) : Room(cugl::Size(width, height)) {}

  Room(cugl::Size size);

  void move(cugl::Vec2 dir);

  bool hasNeighbor(const std::shared_ptr<Room> &room) const;

  std::shared_ptr<Edge> findEdge(const std::shared_ptr<Room> &room) const;

  cugl::Rect getRect() const {
    return cugl::Rect(_node->getPosition(), _node->getSize());
  }

  cugl::Vec2 getMid() const {
    return _node->getPosition() + _node->getSize() / 2.0f;
  }

  float getRadius() const {
    return cugl::Vec2(_node->getSize()).length() / 2.0f;
  }
};

class Edge {
 public:
  std::shared_ptr<Room> _source;
  std::shared_ptr<Room> _neighbor;
  std::shared_ptr<cugl::scene2::PathNode> _path;
  float _weight;
  bool _calculated;

  Edge(const std::shared_ptr<Room> &s, const std::shared_ptr<Room> &n);

  bool doesIntersect(cugl::Vec2 origin, float r);

  std::shared_ptr<Room> getOther(const std::shared_ptr<Room> &room) {
    if (room == _source) {
      return _neighbor;
    } else if (room == _neighbor) {
      return _source;
    }
    return nullptr;
  }

  bool operator==(const Edge &other) const {
    return (this->_source == other._source &&
            this->_neighbor == other._neighbor) ||
           (this->_source == other._neighbor &&
            this->_neighbor == other._source);
  }

  bool shareRoom(const std::shared_ptr<Edge> &other) const {
    return this->_source == other->_source ||
           this->_neighbor == other->_source ||
           this->_neighbor == other->_neighbor ||
           this->_source == other->_neighbor;
  }
};

#endif  // MODELS_ROOM_H