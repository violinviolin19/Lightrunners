#ifndef MODELS_ROOM_H
#define MODELS_ROOM_H
#include <cugl/cugl.h>

class Edge;

class Room {
public:
  enum RoomType { TERMINAL, SPAWN, DEFAULT };

  RoomType type;

  std::shared_ptr<cugl::scene2::PolygonNode> node;

  std::vector<std::shared_ptr<Edge>> edges;

  bool visited;
  bool fixed;

  Room(float x, float y) : Room(cugl::Size(x, y)) {}

  Room(cugl::Size size);

  void move(cugl::Vec2 dir);

  bool hasNeighbor(const std::shared_ptr<Room> room) const;

  std::shared_ptr<Edge> findEdge(const std::shared_ptr<Room> room) const;

  cugl::Rect getRect() const {
    return cugl::Rect(node->getPosition(), node->getSize());
  }

  cugl::Vec2 getMid() const {
    return node->getPosition() + node->getSize() / 2.0f;
  }

  float getRadius() const {
    return cugl::Vec2(node->getSize()).length() / 2.0f;
  }
};

struct Edge {
  std::shared_ptr<Room> source;
  std::shared_ptr<Room> neighbor;
  std::shared_ptr<cugl::scene2::PathNode> path;
  float weight;
  bool active;

  Edge(const std::shared_ptr<Room> &s, const std::shared_ptr<Room> &n);

  bool doesIntersect(cugl::Vec2 origin, float r);

  std::shared_ptr<Room> getOther(const std::shared_ptr<Room> &room) {
    if (room == source) {
      return neighbor;
    } else if (room == neighbor) {
      return source;
    }
    return nullptr;
  }

  bool operator==(const Edge &other) const {
    return (this->source == other.source && this->neighbor == other.neighbor) ||
           (this->source == other.neighbor && this->neighbor == other.source);
  }

  bool shareRoom(const std::shared_ptr<Edge> &other) const {
    return this->source == other->source || this->neighbor == other->source ||
           this->neighbor == other->neighbor || this->source == other->neighbor;
  }
};

#endif