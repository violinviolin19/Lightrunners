#ifndef MODELS_LEVEL_GEN_HALLWAY_H_
#define MODELS_LEVEL_GEN_HALLWAY_H_
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

#endif  // MODELS_LEVEL_GEN_HALLWAY_H_
