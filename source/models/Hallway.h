#ifndef MODELS_HALLWAY_H
#define MODELS_HALLWAY_H
#include <cugl/cugl.h>

#include "Room.h"

class Hallway {
  std::shared_ptr<cugl::scene2::PolygonNode> _node;
  std::shared_ptr<Edge> _edge;

 public:
  Hallway(std::shared_ptr<Edge> edge, float radius);
  ~Hallway(){};

  std::shared_ptr<cugl::scene2::PolygonNode> getNode() const { return _node; }

 private:
  cugl::Vec2 getIntersection(const cugl::Vec2 &start, const cugl::Vec2 &end,
                             const cugl::Rect &rect) const;
};

#endif  // MODELS_HALLWAY_H