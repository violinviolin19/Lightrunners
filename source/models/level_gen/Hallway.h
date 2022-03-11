#ifndef MODELS_LEVEL_GEN_HALLWAY_H_
#define MODELS_LEVEL_GEN_HALLWAY_H_
#include <cugl/cugl.h>

#include "Room.h"

/** A representation of a hallway that connects two Rooms. */
class Hallway {
  /** A reference to the scene2 node for drawing. */
  std::shared_ptr<cugl::scene2::PolygonNode> _node;
  /** A reference to the Edge that this hallway is created from. */
  std::shared_ptr<Edge> _edge;

 public:
  /**
   * Construct a Hallway given the Edge is represents and the radius in grid
   * units.
   * @param edge The edge this hallway represents.
   * @param poly A polygon that represents hallway shape.
   */
  Hallway(std::shared_ptr<Edge> edge, cugl::Poly2 poly);

  /** Destroy the Hallway. */
  ~Hallway(){};

  /** Get the scene2 node used for drawing. */
  std::shared_ptr<cugl::scene2::PolygonNode> getNode() const { return _node; }
};

#endif  // MODELS_LEVEL_GEN_HALLWAY_H_
