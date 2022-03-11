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
   * @param radius The radius of the hallway in grid units.
   */
  Hallway(std::shared_ptr<Edge> edge, float radius);

  /** Destroy the Hallway. */
  ~Hallway(){};

  /** Get the scene2 node used for drawing. */
  std::shared_ptr<cugl::scene2::PolygonNode> getNode() const { return _node; }

 private:
  /**
   * A helper function that checks if a line segment with given start and end
   * values intersects the given rectangle.
   * @param start The start point of the line segment.
   * @param end The end point of the line segment.
   * @param rect A rectangle that we are checking for collision.
   * @return The point of intersection between the line segment and the
   * rectangle. Returns a (0,0) vector if an intersection is not found.
   */
  cugl::Vec2 getIntersection(const cugl::Vec2 &start, const cugl::Vec2 &end,
                             const cugl::Rect &rect) const;
};

#endif  // MODELS_LEVEL_GEN_HALLWAY_H_
