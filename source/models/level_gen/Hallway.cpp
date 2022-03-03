#include "Hallway.h"

Hallway::Hallway(std::shared_ptr<Edge> edge, float radius) {
  cugl::Vec2 start =
      getIntersection(edge->_source->getMid(), edge->_neighbor->getMid(),
                      edge->_source->getRect());
  cugl::Vec2 end =
      getIntersection(edge->_source->getMid(), edge->_neighbor->getMid(),
                      edge->_neighbor->getRect());

  cugl::Vec2 disp = start - end;
  cugl::Vec2 midpoint = (start + end) / 2.0f;

  std::vector<cugl::Vec2> vertices;
  if (disp.x == 0 || disp.y == 0) {
    vertices.push_back(start);
    vertices.push_back(end);
  } else if (abs(disp.x) == 1) {
    vertices.push_back(start);
    vertices.push_back(cugl::Vec2(start.x, end.y));
  } else if (abs(disp.y) == 1) {
    vertices.push_back(start);
    vertices.push_back(cugl::Vec2(end.x, start.y));
  } else if (abs(disp.x) > abs(disp.y)) {
    vertices.push_back(start);
    vertices.push_back(cugl::Vec2(floorf(midpoint.x) + 0.5f, start.y));
    vertices.push_back(cugl::Vec2(floorf(midpoint.x) + 0.5f, end.y));
    vertices.push_back(end);
  } else if (abs(disp.x) <= abs(disp.y)) {
    vertices.push_back(start);
    vertices.push_back(cugl::Vec2(start.x, floorf(midpoint.y) + 0.5f));
    vertices.push_back(cugl::Vec2(end.x, floorf(midpoint.y) + 0.5f));
    vertices.push_back(end);
  }

  cugl::Path2 path(vertices);

  cugl::SimpleExtruder simple_extruder;
  simple_extruder.set(path);
  simple_extruder.setEndCap(cugl::poly2::EndCap::BUTT);
  simple_extruder.setJoint(cugl::poly2::Joint::MITRE);
  simple_extruder.calculate(radius);

  _node =
      cugl::scene2::PolygonNode::allocWithPoly(simple_extruder.getPolygon());

  _node->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);
  _node->setPosition(_node->getBoundingRect().origin);
  _node->setColor(cugl::Color4(0, 0, 0, 100));
}

cugl::Vec2 Hallway::getIntersection(const cugl::Vec2 &start,
                                    const cugl::Vec2 &end,
                                    const cugl::Rect &rect) const {
  // Vertices in clockwise order.
  std::vector<cugl::Vec2> vertices{cugl::Vec2(rect.getMinX(), rect.getMaxY()),
                                   cugl::Vec2(rect.getMaxX(), rect.getMaxY()),
                                   cugl::Vec2(rect.getMaxX(), rect.getMinY()),
                                   cugl::Vec2(rect.getMinX(), rect.getMinY()),
                                   cugl::Vec2(rect.getMinX(), rect.getMaxY())};

  for (int i = 0; i < vertices.size() - 1; i++) {
    float S, T;
    bool intersects = cugl::Vec2::doesLineIntersect(start, end, vertices[i],
                                                    vertices[i + 1], &S, &T);
    // Make sure that the we're checking the segments and not the lines made
    // by the points. S & T must be in range [0,1].
    if (intersects && (S >= 0.0f && S <= 1.0f && T >= 0.0f && T <= 1.0f)) {
      cugl::Vec2 res = start + (end - start) * S;

      if (i % 2 == 0 /* == Intersect on horizontal rectangle face. */) {
        res.x = floorf(res.x) + 0.5f;
        res.x -= (res.x > rect.getMaxX()) ? 1.0f : 0.0f;
      } else {
        res.y = floorf(res.y) + 0.5f;
        res.y -= (res.y > rect.getMaxY()) ? 1.0f : 0.0f;
      }

      return res;
    }
  }
  return cugl::Vec2::ZERO;
}
