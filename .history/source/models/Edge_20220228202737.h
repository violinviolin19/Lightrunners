#ifndef MODELS_EDGE_H
#define MODELS_EDGE_H
#include <cugl/cugl.h>

class Room;

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