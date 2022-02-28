#ifndef GENERATORS_LEVEL_GENERATOR_H
#define GENERATORS_LEVEL_GENERATOR_H
#include "LevelGeneratorConfig.h"
#include <cugl/cugl.h>

struct Edge;

struct Room {

  std::shared_ptr<cugl::scene2::PolygonNode> node;
  std::vector<Edge> edges;
  bool visited;
  bool fixed;

  Room() : Room(cugl::Size::ZERO) {}

  Room(float x, float y) : Room(cugl::Size(x, y)){};

  Room(cugl::Size size);

  void move(cugl::Vec2 dir);

  const cugl::Rect getRect() {
    cugl::Rect rect(node->getPosition(), node->getSize());
    return rect;
  }

  const cugl::Vec2 getMid() {
    return (node->getPosition() + node->getSize()) / 2.0f;
  }

  float getRadius() { return cugl::Vec2(node->getSize()).length() / 2.0f; }

  bool operator==(const Room &other) const;

  bool operator!=(const Room &other) const;
};

struct Edge {
  Room source;
  Room neighbor;
  std::shared_ptr<cugl::scene2::PathNode> path;
  float weight;

  Edge(Room &s, Room &n);

  bool operator==(const Edge &other) const;

  bool doesIntersect(cugl::Vec2 origin, float r);
};

class LevelGenerator {
private:
  std::shared_ptr<cugl::scene2::SceneNode> _map;
  LevelGeneratorConfig _config;
  std::vector<Room> _rooms;
  Room _spawn_room;
  bool _active;
  std::function<void(void)> _generator_step;

public:
  static const cugl::Size TERMINAL;
  static const cugl::Size SPAWN;

  LevelGenerator();
  ~LevelGenerator() {}

  std::vector<Room> getRooms() const { return _rooms; }

  void init(LevelGeneratorConfig config,
            const std::shared_ptr<cugl::scene2::SceneNode> &map);

  void dispose();

  void generateRooms();

  void separateRooms(std::function<void(void)> next_generator_step = nullptr);

  bool anyRoomsOverlapping();

  Room roomMostOverlappingWith(Room &room);

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

  void calculateDelaunayTriangles(std::vector<Room> rooms,
                                  float inner_circle_cutoff_radius);

  void calculateMinimumSpanningTree();

  void addEdgesBack();

  void fillHallways();
};

#endif /* GENERATORS_LEVELGENERATOR_H */
