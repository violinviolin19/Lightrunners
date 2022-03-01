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

#endif