#include "LevelGenerator.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcomma"
#include "Delaunator.h"
#pragma GCC diagnostic pop

#include <cugl/cugl.h>

// static
const cugl::Size LevelGenerator::TERMINAL(10.0f, 10.0f);

// static
const cugl::Size LevelGenerator::SPAWN(10.0f, 10.0f);

LevelGenerator::LevelGenerator() : _active(false), _generator_step(nullptr) {}

void LevelGenerator::init(LevelGeneratorConfig &config,
                          const std::shared_ptr<cugl::scene2::SceneNode> &map) {
  if (!_active) {
    _config = config;
    _map = map;
    _generator_step = [this]() { this->generateRooms(); };
    std::random_device my_random_device;
    unsigned seed = my_random_device();
    _generator = std::default_random_engine(seed);
    _active = true;
  }
}

void LevelGenerator::dispose() {
  if (_active) {
    _rooms.clear();
    _inside_rooms.clear();
    _middle_rooms.clear();
    _outside_rooms.clear();
    _spawn_room = nullptr;
    _map = nullptr;
    _generator_step = nullptr;
    _active = false;
  }
}

void LevelGenerator::update() {
  if (_generator_step != nullptr) {
    nextStep();
  }
}

void LevelGenerator::generateRooms() {

  _spawn_room = std::make_shared<Room>(LevelGenerator::SPAWN);
  _spawn_room->type = Room::RoomType::SPAWN;
  _spawn_room->fixed = true;
  _spawn_room->node->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);
  _spawn_room->node->setPosition(_spawn_room->node->getContentSize() / -2.0f);
  _spawn_room->node->setColor(cugl::Color4(14, 14, 205, 127));
  _rooms.push_back(_spawn_room);
  _inside_rooms.push_back(_spawn_room);
  _map->addChild(_spawn_room->node);

  float min_radius = _spawn_room->getRadius();

  placeRegularRooms(_config.getNumRooms(), min_radius,
                    _config.getMiddleCircleRadius());

  _generator_step = [this]() {
    this->separateRooms([this]() { this->placeTerminals(); });
  };
}

void LevelGenerator::placeRegularRooms(int num_rooms, float min_radius,
                                       float max_radius) {
  //  Distribution to define the size of normal rooms.
  std::normal_distribution<float> size_dis(9.0f, 1.0f);
  std::uniform_real_distribution<float> dis(0.0f, 1.0f);

  for (float i = 0; i < num_rooms; i++) {
    std::shared_ptr<Room> room = std::make_shared<Room>(
        floor(size_dis(_generator)), floor(size_dis(_generator)));

    float room_radius = room->getRadius();
    float min_r = min_radius + room_radius;
    float max_r = max_radius - room_radius;

    float r = dis(_generator) * (max_r - min_r) + min_r;
    float angle = dis(_generator) * 2 * M_PI;
    cugl::Vec2 pos(floor(r * cosf(angle)), floor(r * sinf(angle)));

    room->node->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);
    room->node->setPosition(pos - (room->node->getContentSize() / 2.0f));
    room->node->setColor(cugl::Color4(125, 94, 52, 127));

    _rooms.push_back(room);
    _map->addChild(room->node);
  }
}

void LevelGenerator::separateRooms(
    std::function<void(void)> next_generator_step) {
  if (!anyRoomsOverlapping()) {
    _generator_step = next_generator_step;
    return;
  }
  for (int i = 0; i < _rooms.size(); i++) {
    std::shared_ptr<Room> &room = _rooms[i];
    cugl::Rect room_rect = room->getRect();

    for (int j = i + 1; j < _rooms.size(); j++) {
      std::shared_ptr<Room> &n_room = _rooms[j];
      cugl::Rect n_room_rect = n_room->getRect();

      if (room_rect.doesIntersect(n_room_rect)) {
        cugl::Vec2 direction = room->getMid() - n_room->getMid();
        if (direction == cugl::Vec2::ZERO) {
          direction += cugl::Vec2::ONE;
        }
        direction.normalize();
        room->move(direction);
        n_room->move(direction * -1.0f);
      }
    }
  }
}

bool LevelGenerator::anyRoomsOverlapping() {
  for (int i = 0; i < _rooms.size(); i++) {
    cugl::Rect room_rect = _rooms[i]->getRect();
    for (int j = i + 1; j < _rooms.size(); j++) {
      if (_rooms[i] != _rooms[j]) {
        cugl::Rect n_room_rect = _rooms[j]->getRect();

        if (room_rect.doesIntersect(n_room_rect)) {
          return true;
        }
      }
    }
  }
  return false;
}

std::shared_ptr<Room>
LevelGenerator::roomMostOverlappingWith(const std::shared_ptr<Room> &room) {
  cugl::Rect room_rect = room->getRect();

  auto it = std::max_element(
      _rooms.begin(), _rooms.end(),
      [room_rect](const std::shared_ptr<Room> &l,
                  const std::shared_ptr<Room> &r) {
        cugl::Size l_size = l->getRect().intersect(room_rect).size;
        cugl::Size r_size = r->getRect().intersect(room_rect).size;
        return l_size.width * l_size.height < r_size.width * r_size.height;
      });

  return (it != _rooms.end()) ? (*it) : room;
}

void LevelGenerator::placeTerminals() {
  float min_radius = _spawn_room->getRadius();

  std::vector<std::shared_ptr<Room>> inner_terminals =
      placeTerminalRooms(_config.getNumTerminalRoomsInner(), min_radius,
                         _config.getInnerCircleRadius());
  std::vector<std::shared_ptr<Room>> middle_terminals = placeTerminalRooms(
      _config.getNumTerminalRoomsMiddle(), _config.getInnerCircleRadius(),
      _config.getMiddleCircleRadius());
  std::vector<std::shared_ptr<Room>> outer_terminals = placeTerminalRooms(
      _config.getNumTerminalRoomsOuter(), _config.getMiddleCircleRadius(),
      _config.getMapRadius());

  _inside_rooms.insert(_inside_rooms.end(), inner_terminals.begin(),
                       inner_terminals.end());
  _middle_rooms.insert(_middle_rooms.end(), middle_terminals.begin(),
                       middle_terminals.end());
  _outside_rooms.insert(_outside_rooms.end(), outer_terminals.begin(),
                        outer_terminals.end());

  _generator_step = [this]() {
    this->separateRooms([this]() { this->segregateLayers(); });
  };
}

std::vector<std::shared_ptr<Room>>
LevelGenerator::placeTerminalRooms(int num_rooms, float min_radius,
                                   float max_radius) {

  std::uniform_real_distribution<float> dis(0.0f, 1.0f);

  // Make sure the room is always inside of the spawn circle;
  float terminal_radius =
      static_cast<cugl::Vec2>(LevelGenerator::TERMINAL).length() / 2.0f;
  min_radius += terminal_radius;
  max_radius -= terminal_radius;

  float r = dis(_generator) * (max_radius - min_radius) + min_radius;
  float min_angle = dis(_generator) * 2 * M_PI;
  float max_angle = min_angle + M_PI / 2.0f;

  std::vector<std::shared_ptr<Room>> terminals;

  for (float i = 0; i < num_rooms; i++) {
    std::shared_ptr<Room> room =
        std::make_shared<Room>(LevelGenerator::TERMINAL);

    room->type = Room::RoomType::TERMINAL;

    room->node->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);

    float angle = dis(_generator) * (max_angle - min_angle) + min_angle;
    cugl::Vec2 pos(floor(r * cosf(angle)), floor(r * sinf(angle)));

    room->node->setPosition(pos - (room->node->getContentSize() / 2.0f));
    room->node->setColor(cugl::Color4(52, 205, 14, 127));

    min_angle += 2 * M_PI / num_rooms;
    max_angle += 2 * M_PI / num_rooms;

    _rooms.push_back(room);
    terminals.push_back(room);
    _map->addChild(room->node);

    std::shared_ptr<Room> overlapping = roomMostOverlappingWith(room);
    if (overlapping != room) {
      overlapping->node->setColor(cugl::Color4(205, 14, 14, 127));
      auto it = std::find(_rooms.begin(), _rooms.end(), overlapping);
      if (it != _rooms.end()) {
        _map->removeChild(overlapping->node);
        *it = room;
      }
    }
  }

  return terminals;
}

void LevelGenerator::segregateLayers() {
  std::vector<std::shared_ptr<Room>> inside_rooms;
  std::copy_if(_rooms.begin(), _rooms.end(), std::back_inserter(inside_rooms),
               [this](const std::shared_ptr<Room> &room) {
                 return room->type == Room::RoomType::DEFAULT &&
                        room->getMid().length() <=
                            this->_config.getInnerCircleRadius();
               });

  std::vector<std::shared_ptr<Room>> middle_rooms;
  std::copy_if(_rooms.begin(), _rooms.end(), std::back_inserter(middle_rooms),
               [this](const std::shared_ptr<Room> &room) {
                 float r = room->getMid().length();
                 return room->type == Room::RoomType::DEFAULT &&
                        r > this->_config.getInnerCircleRadius() &&
                        r <= this->_config.getMiddleCircleRadius();
               });

  std::vector<std::shared_ptr<Room>> outside_rooms;
  std::copy_if(_rooms.begin(), _rooms.end(), std::back_inserter(outside_rooms),
               [this](const std::shared_ptr<Room> &room) {
                 return room->type == Room::RoomType::DEFAULT &&
                        room->getMid().length() >
                            this->_config.getMiddleCircleRadius();
               });

  _inside_rooms.insert(_inside_rooms.end(), inside_rooms.begin(),
                       inside_rooms.end());
  _middle_rooms.insert(_middle_rooms.end(), middle_rooms.begin(),
                       middle_rooms.end());
  _outside_rooms.insert(_outside_rooms.end(), outside_rooms.begin(),
                        outside_rooms.end());

  for (std::shared_ptr<Room> &room : _middle_rooms) {
    cugl::Vec2 pos = room->getMid();
    pos += room->getMid().getNormalization() *
           _config.getSeparationBetweenLayers();
    pos -= room->node->getSize() / 2.0f;

    room->node->setPosition(roundf(pos.x), roundf(pos.y));
  }

  for (std::shared_ptr<Room> &room : _outside_rooms) {
    cugl::Vec2 pos = room->getMid();
    pos += room->getMid().getNormalization() *
           _config.getSeparationBetweenLayers() * 2.0f;
    pos -= room->node->getSize() / 2.0f;

    room->node->setPosition(roundf(pos.x), roundf(pos.y));
  }

  _generator_step = [this]() {
    this->separateRooms([this]() { this->markAndFillHallways(); });
  };
}

void LevelGenerator::markAndFillHallways() {
  calculateDelaunayTriangles(_inside_rooms, 0.0f);
  calculateDelaunayTriangles(_middle_rooms, _config.getInnerCircleRadius());
  calculateDelaunayTriangles(_outside_rooms, _config.getMiddleCircleRadius());

  calculateMinimumSpanningTree(_inside_rooms);
  calculateMinimumSpanningTree(_middle_rooms);
  calculateMinimumSpanningTree(_outside_rooms);

  addEdgesBack(_inside_rooms);
  addEdgesBack(_middle_rooms);
  addEdgesBack(_outside_rooms);

  connectLayers(_inside_rooms, _middle_rooms, 2);
  connectLayers(_middle_rooms, _outside_rooms, 3);

  fillHallways();

  _generator_step = [this]() { this->buildCompositeAreas(); };
}

void LevelGenerator::buildCompositeAreas() {
  _generator_step = [this]() { this->establishGates(); };
}

void LevelGenerator::establishGates() { _generator_step = nullptr; }

void LevelGenerator::calculateDelaunayTriangles(
    std::vector<std::shared_ptr<Room>> &rooms, float r) {

  if (rooms.size() == 0)
    return;

  std::vector<double> coords;

  for (std::shared_ptr<Room> &room : rooms) {
    cugl::Vec2 mid_point = room->getMid();

    coords.push_back(static_cast<double>(mid_point.x));
    coords.push_back(static_cast<double>(mid_point.y));
  }

  delaunator::Delaunator d(coords);

  for (std::size_t i = 0; i < d.triangles.size(); i += 3) {
    std::shared_ptr<Room> &node_0 = rooms[d.triangles[i]];
    std::shared_ptr<Room> &node_1 = rooms[d.triangles[i + 1]];
    std::shared_ptr<Room> &node_2 = rooms[d.triangles[i + 2]];

    std::shared_ptr<Edge> edge_0_1 = std::make_shared<Edge>(node_0, node_1);
    if (r == 0.0f || !edge_0_1->doesIntersect(cugl::Vec2::ZERO, r)) {

      auto adjacent_0_1 = node_0->findEdge(node_1);
      auto adjacent_1_0 = node_1->findEdge(node_0);

      if (!adjacent_0_1 && !adjacent_1_0) {
        _map->addChild(edge_0_1->path);
        node_0->edges.push_back(edge_0_1);
        node_1->edges.push_back(edge_0_1);
      } else if (!adjacent_0_1) {
        node_0->edges.push_back(adjacent_1_0);
      } else if (!adjacent_1_0) {
        node_1->edges.push_back(adjacent_0_1);
      }
    }

    std::shared_ptr<Edge> edge_0_2 = std::make_shared<Edge>(node_0, node_2);
    if (r == 0.0f || !edge_0_2->doesIntersect(cugl::Vec2::ZERO, r)) {
      auto adjacent_0_2 = node_0->findEdge(node_2);
      auto adjacent_2_0 = node_2->findEdge(node_0);

      if (!adjacent_0_2 && !adjacent_2_0) {
        _map->addChild(edge_0_2->path);
        node_0->edges.push_back(edge_0_2);
        node_2->edges.push_back(edge_0_2);
      } else if (!adjacent_0_2) {
        node_0->edges.push_back(adjacent_2_0);
      } else if (!adjacent_2_0) {
        node_1->edges.push_back(adjacent_0_2);
      }
    }

    std::shared_ptr<Edge> edge_1_2 = std::make_shared<Edge>(node_1, node_2);
    if (r == 0.0f || !edge_1_2->doesIntersect(cugl::Vec2::ZERO, r)) {
      auto adjacent_1_2 = node_1->findEdge(node_2);
      auto adjacent_2_1 = node_2->findEdge(node_1);

      if (!adjacent_1_2 && !adjacent_2_1) {
        _map->addChild(edge_1_2->path);
        node_1->edges.push_back(edge_1_2);
        node_2->edges.push_back(edge_1_2);
      } else if (!adjacent_1_2) {
        node_1->edges.push_back(adjacent_2_1);
      } else if (!adjacent_2_1) {
        node_2->edges.push_back(adjacent_1_2);
      }
    }
  }

  _map->doLayout();
}

void LevelGenerator::calculateMinimumSpanningTree(
    std::vector<std::shared_ptr<Room>> &rooms) {
  int num_edges = 0;
  std::vector<std::shared_ptr<Edge>> result;
  // Reset tree to start Prim's algorithm.
  for (std::shared_ptr<Room> &room : rooms) {
    room->visited = false;
    for (std::shared_ptr<Edge> edge : room->edges) {
      edge->path->setVisible(false);
      edge->active = false;
    }
  }
  rooms[0]->visited = true;

  while (num_edges < rooms.size() - 1) {
    std::shared_ptr<Edge> min_edge = nullptr;
    std::shared_ptr<Room> min_neighbor = nullptr;
    float min_weight = FLT_MAX;

    for (std::shared_ptr<Room> &room : rooms) {
      if (room->visited) {
        for (std::shared_ptr<Edge> edge : room->edges) {
          if (!edge->getOther(room)->visited && edge->weight < min_weight) {
            min_edge = edge;
            min_neighbor = edge->getOther(room);
            min_weight = edge->weight;
          }
        }
      }
    }

    num_edges++;

    if (min_edge) {
      min_neighbor->visited = true;
      result.push_back(std::move(min_edge));
    }
  }

  for (std::shared_ptr<Edge> &edge : result) {
    edge->path->setColor(cugl::Color4(255, 14, 14, 124));
    edge->path->setVisible(true);
    edge->active = true;
  }
}

void LevelGenerator::addEdgesBack(std::vector<std::shared_ptr<Room>> &rooms) {
  std::uniform_real_distribution<float> rand(0.0f, 1.0f);
  for (std::shared_ptr<Room> &room : rooms) {
    for (std::shared_ptr<Edge> edge : room->edges) {
      if (!edge->active && edge->weight < _config.getMaxHallwayLength() &&
          rand(_generator) <= _config.getAddEdgesBackProb()) {
        edge->active = true;
        edge->path->setVisible(true);
        edge->path->setColor(cugl::Color4(255, 14, 14, 124));
      }
    }
  }
}

void LevelGenerator::connectLayers(std::vector<std::shared_ptr<Room>> &layer_a,
                                   std::vector<std::shared_ptr<Room>> &layer_b,
                                   int num_connections) {

  std::vector<std::shared_ptr<Edge>> connections;

  std::uniform_real_distribution<float> dis(0.0f, 1.0f);
  float min_angle = dis(_generator) * 2 * M_PI;
  float max_angle = fmod(min_angle + M_PI / num_connections, 2 * M_PI);

  for (int i = 0; i < num_connections; i++) {
    std::shared_ptr<Edge> winner;
    float winner_dist = FLT_MAX;

    for (std::shared_ptr<Room> &a_room : layer_a) {
      float angle = a_room->getMid().getAngle();
      angle += (angle < 0.0f) ? (2 * M_PI) : 0.0f;

      bool between_angles = (max_angle > min_angle)
                                ? (angle >= min_angle && angle <= max_angle)
                                : (angle >= min_angle || angle <= max_angle);

      if (between_angles && a_room->type == Room::RoomType::DEFAULT) {
        for (std::shared_ptr<Room> &b_room : layer_b) {
          if (b_room->type == Room::RoomType::DEFAULT) {

            // Find if this edge already been chosen.
            auto curr = std::make_shared<Edge>(a_room, b_room);
            auto res =
                std::find_if(connections.begin(), connections.end(),
                             [curr](const std::shared_ptr<Edge> &edge) {
                               return *edge == *curr || edge->shareRoom(curr);
                             });

            if (curr->weight < winner_dist && res == connections.end()) {
              winner_dist = curr->weight;
              winner = std::move(curr);
            }
          }
        }
      }
    }

    if (winner) {
      connections.push_back(winner);
      min_angle = fmod(min_angle + 2 * M_PI / num_connections, 2 * M_PI);
      max_angle = fmod(min_angle + M_PI / num_connections, 2 * M_PI);
    }
  }

  for (std::shared_ptr<Edge> &connection : connections) {
    connection->source->edges.push_back(connection);
    connection->neighbor->edges.push_back(connection);
    connection->path->setColor(cugl::Color4(15, 15, 230, 147));
    _map->addChild(connection->path);
  }
  _map->doLayout();
}

void LevelGenerator::fillHallways() {}