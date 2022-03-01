#ifndef GENERATORS_LEVEL_GENERATOR_H
#define GENERATORS_LEVEL_GENERATOR_H
#include "../models/Room.h"
#include "LevelGeneratorConfig.h"
#include <cugl/cugl.h>

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

  void segregateLayers();

  void calculateDelaunayTriangles(std::vector<std::shared_ptr<Room>> &rooms,
                                  float r);

  void calculateMinimumSpanningTree(std::vector<std::shared_ptr<Room>> &rooms);

  void addEdgesBack(std::vector<std::shared_ptr<Room>> &rooms);

  void connectLayers(std::vector<std::shared_ptr<Room>> &layer_a,
                     std::vector<std::shared_ptr<Room>> &layer_b,
                     int num_connections);

  void fillHallways();
};

#endif /* GENERATORS_LEVELGENERATOR_H */
