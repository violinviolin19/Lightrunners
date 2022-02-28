#ifndef GENERATORS_LEVEL_GENERATOR_H
#define GENERATORS_LEVEL_GENERATOR_H
#include "LevelGeneratorConfig.h"
#include <cugl/cugl.h>

struct Edge;

struct Room {

  std::shared_ptr<cugl::scene2::PolygonNode> node;
  std::vector<std::shared_ptr<Edge>> edges;
  bool visited;
  bool fixed;

  Room(float x, float y) : Room(cugl::Size(x, y)){};

  Room(cugl::Size size);

  void move(cugl::Vec2 dir);

  cugl::Rect getRect() {
    cugl::Rect rect(node->getPosition(), node->getSize());
    return rect;
  }

  cugl::Vec2 getMid() { return (node->getPosition() + node->getSize()) / 2.0f; }

  float getRadius() { return cugl::Vec2(node->getSize()).length() / 2.0f; }
};

struct Edge {
  std::shared_ptr<Room> source;
  std::shared_ptr<Room> neighbor;
  std::shared_ptr<cugl::scene2::PathNode> path;
  float weight;

  Edge(std::shared_ptr<Room> s, std::shared_ptr<Room> n)
      : source(s), neighbor(n), weight(0) {
    std::vector<cugl::Vec2> vertices{source->getMid(), neighbor->getMid()};
    path = cugl::scene2::PathNode::allocWithVertices(vertices, 0.4f);
    path->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);
    path->setPosition(path->getBoundingRect().origin);
    path->setColor(cugl::Color4(0, 0, 0, 50));
    weight = (vertices[1] - vertices[0]).length();
  }

  bool operator==(const Edge &other) const {
    return (this->source == other.source && this->neighbor == other.neighbor) ||
           (this->source == other.neighbor && this->neighbor == other.source);
  }

  bool doesIntersect(cugl::Vec2 origin, float r) {

    cugl::Vec2 d = neighbor->getMid() - source->getMid();
    cugl::Vec2 f = source->getMid() - origin;

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
};

class LevelGenerator {
private:
  std::shared_ptr<cugl::scene2::SceneNode> _map;
  LevelGeneratorConfig _config;
  std::vector<std::shared_ptr<Room>> _rooms;
  std::shared_ptr<Room> _spawn_room;
  bool _active;
  std::function<void(void)> _generator_step;

public:
  static const cugl::Size TERMINAL;
  static const cugl::Size SPAWN;

  LevelGenerator();
  ~LevelGenerator() {}

  std::vector<std::shared_ptr<Room>> getRooms() const { return _rooms; }

  void init(LevelGeneratorConfig config,
            std::shared_ptr<cugl::scene2::SceneNode> map);

  void dispose();

  void generateRooms();

  void separateRooms(std::function<void(void)> next_generator_step = nullptr);

  bool anyRoomsOverlapping();

  std::shared_ptr<Room> roomMostOverlappingWith(std::shared_ptr<Room> room);

  void markAndFillHallways();

  void buildCompositeAreas();

  void establishGates();

  std::function<void(void)> getNextStep() { return _generator_step; }

private:
  void placeTerminalRooms(int num_rooms, float min_radius, float max_radius,
                          std::default_random_engine &generator);

  void placeRegularRooms(int num_rooms, float min_radius, float max_radius,
                         std::default_random_engine &generator);

  void placeTerminals();

  void calculateDelaunayTriangles(std::vector<std::shared_ptr<Room>> rooms,
                                  float inner_circle_cutoff_radius);

  void calculateMinimumSpanningTree();

  void addEdgesBack();

  void fillHallways();
};

#endif /* GENERATORS_LEVELGENERATOR_H */
