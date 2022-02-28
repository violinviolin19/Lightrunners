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

  Room(float x, float y) : Room(cugl::Size(x, y)) {}

  Room(cugl::Size size);

  void move(cugl::Vec2 dir);

  bool hasNeighbor(const std::shared_ptr<Room> room) const;

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
};

class LevelGenerator {
private:
  std::shared_ptr<cugl::scene2::SceneNode> _map;
  LevelGeneratorConfig _config;

  std::vector<std::shared_ptr<Room>> _rooms;
  std::vector<std::shared_ptr<Room>> _inside_rooms;
  std::vector<std::shared_ptr<Room>> _middle_rooms;
  std::vector<std::shared_ptr<Room>> _outside_rooms;

  std::shared_ptr<Room> _spawn_room;
  bool _active;
  std::function<void(void)> _generator_step;
  std::default_random_engine _generator;

public:
  static const cugl::Size TERMINAL;
  static const cugl::Size SPAWN;

  LevelGenerator();
  ~LevelGenerator() {}

  std::vector<std::shared_ptr<Room>> getRooms() const { return _rooms; }

  void init(LevelGeneratorConfig &config,
            const std::shared_ptr<cugl::scene2::SceneNode> &map);

  void dispose();

  void generateRooms();

  void separateRooms(std::function<void(void)> next_generator_step = nullptr);

  bool anyRoomsOverlapping();

  std::shared_ptr<Room>
  roomMostOverlappingWith(const std::shared_ptr<Room> &room);

  void markAndFillHallways();

  void buildCompositeAreas();

  void establishGates();

  std::function<void(void)> getNextStep() { return _generator_step; }

private:
  std::vector<std::shared_ptr<Room>>
  placeTerminalRooms(int num_rooms, float min_radius, float max_radius);

  void placeRegularRooms(int num_rooms, float min_radius, float max_radius);

  void placeTerminals();

  void calculateDelaunayTriangles(std::vector<std::shared_ptr<Room>> &rooms,
                                  float inner_circle_cutoff_radius);

  void calculateMinimumSpanningTree(std::vector<std::shared_ptr<Room>> &rooms);

  void addEdgesBack(std::vector<std::shared_ptr<Room>> &rooms);

  void connectLayers(std::vector<std::shared_ptr<Room>> &layer_a,
                     std::vector<std::shared_ptr<Room>> &layer_b);

  void fillHallways();
};

#endif /* GENERATORS_LEVELGENERATOR_H */
