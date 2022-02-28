#include "LevelGenerator.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcomma"
#include "Delaunator.h"
#pragma GCC diagnostic pop

#include <cugl/cugl.h>

#pragma mark Room

Room::Room(cugl::Size size) : visited(false), fixed(false) {
  node = cugl::scene2::PolygonNode::alloc();
  node->setContentSize(size);
}

void Room::move(cugl::Vec2 dir) {
  if (!fixed) {
    dir.set(roundf(dir.x), roundf(dir.y));
    node->setPosition(node->getPosition() + dir);
  }
}

bool Room::operator==(const Room &other) const {
  return (this->node == other.node);
}

bool Room::operator!=(const Room &other) const {
  return !(this->node == other.node);
}

#pragma mark Edge

Edge::Edge(Room &s, Room &n) : source(s), neighbor(n), weight(0) {
  std::vector<cugl::Vec2> vertices{source.getMid(), neighbor.getMid()};
  path = cugl::scene2::PathNode::allocWithVertices(vertices, 0.4f);
  path->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);
  path->setPosition(path->getBoundingRect().origin);
  path->setColor(cugl::Color4(0, 0, 0, 50));
  weight = (vertices[1] - vertices[0]).length();
}

bool Edge::operator==(const Edge &other) const {
  return (this->source == other.source && this->neighbor == other.neighbor) ||
         (this->source == other.neighbor && this->neighbor == other.source);
}

bool Edge::doesIntersect(cugl::Vec2 origin, float r) {

  cugl::Vec2 d = neighbor.getMid() - source.getMid();
  cugl::Vec2 f = source.getMid() - origin;

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

#pragma mark LevelGenerator

// static
const cugl::Size LevelGenerator::TERMINAL(10.0f, 10.0f);

// static
const cugl::Size LevelGenerator::SPAWN(10.0f, 10.0f);

LevelGenerator::LevelGenerator() : _active(false), _generator_step(nullptr) {}

void LevelGenerator::init(LevelGeneratorConfig config,
                          const std::shared_ptr<cugl::scene2::SceneNode> &map) {
  if (!_active) {
    _config = config;
    _active = true;
    _map = map;
    _generator_step = [this]() { this->generateRooms(); };
  }
}

void LevelGenerator::dispose() {
  if (_active) {
    _rooms.clear();
    _active = false;
    _generator_step = nullptr;
  }
}

void LevelGenerator::generateRooms() {

  std::random_device my_random_device;
  unsigned seed = my_random_device();
  std::default_random_engine gen(seed);

  _spawn_room = Room(LevelGenerator::SPAWN);
  _spawn_room.fixed = true;
  _spawn_room.node->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);
  _spawn_room.node->setPosition(_spawn_room.node->getContentSize() / -2.0f);
  _spawn_room.node->setColor(cugl::Color4(14, 14, 205, 127));
  _rooms.push_back(_spawn_room);
  _map->addChild(_spawn_room.node);

  float min_radius = _spawn_room.getRadius();

  placeRegularRooms(_config.getNumRooms(), min_radius,
                    _config.getMiddleCircleRadius(), gen);

  _generator_step = [this]() {
    this->separateRooms([this]() { this->placeTerminals(); });
  };
}

void LevelGenerator::placeRegularRooms(int num_rooms, float min_radius,
                                       float max_radius,
                                       std::default_random_engine &generator) {
  //  Distribution to define the size of normal rooms.
  std::normal_distribution<float> size_dis(9.0f, 1.0f);
  std::uniform_real_distribution<float> dis(0.0f, 1.0f);

  for (float i = 0; i < num_rooms; i++) {
    Room room(floor(size_dis(generator)), floor(size_dis(generator)));

    float room_radius = room.getRadius();
    float min_r = min_radius + room_radius;
    float max_r = max_radius - room_radius;

    float r = dis(generator) * (max_r - min_r) + min_r;
    float angle = dis(generator) * 2 * M_PI;
    cugl::Vec2 pos(floor(r * cosf(angle)), floor(r * sinf(angle)));

    room.node->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);
    room.node->setPosition(pos - (room.node->getContentSize() / 2.0f));
    room.node->setColor(cugl::Color4(125, 94, 52, 127));

    _rooms.push_back(room);
    _map->addChild(room.node);
  }
}

void LevelGenerator::separateRooms(
    std::function<void(void)> next_generator_step) {
  if (!anyRoomsOverlapping()) {
    _generator_step = next_generator_step;
    return;
  }
  for (int i = 0; i < _rooms.size(); i++) {
    Room &room = _rooms[i];
    cugl::Rect room_rect = room.getRect();

    for (int j = i + 1; j < _rooms.size(); j++) {
      Room &n_room = _rooms[j];
      cugl::Rect n_room_rect = n_room.getRect();

      if (room_rect.doesIntersect(n_room_rect)) {
        cugl::Vec2 direction = room.getMid() - n_room.getMid();
        if (direction == cugl::Vec2::ZERO) {
          direction += cugl::Vec2::ONE;
        }
        direction.normalize();
        room.move(direction);
        n_room.move(direction * -1.0f);
      }
    }
  }
}

bool LevelGenerator::anyRoomsOverlapping() {
  for (int i = 0; i < _rooms.size(); i++) {
    cugl::Rect room_rect = _rooms[i].getRect();
    for (int j = i + 1; j < _rooms.size(); j++) {
      if (_rooms[i] != _rooms[j]) {
        cugl::Rect n_room_rect = _rooms[j].getRect();

        if (room_rect.doesIntersect(n_room_rect)) {
          return true;
        }
      }
    }
  }
  return false;
}

Room LevelGenerator::roomMostOverlappingWith(Room &room) {
  cugl::Rect room_rect = room.getRect();

  auto it = std::max_element(
      _rooms.begin(), _rooms.end(), [room_rect](Room const &l, Room const &r) {
        cugl::Size l_size = l.getRect().intersect(room_rect).size;
        cugl::Size r_size = r.getRect().intersect(room_rect).size;
        return l_size.width * l_size.height < r_size.width * r_size.height;
      });

  return (it != _rooms.end()) ? (*it) : room;
}

void LevelGenerator::placeTerminals() {
  std::random_device my_random_device;
  unsigned seed = my_random_device();
  std::default_random_engine gen(seed);

  float min_radius = _spawn_room.getRadius();

  placeTerminalRooms(_config.getNumTerminalRoomsInner(), min_radius,
                     _config.getInnerCircleRadius(), gen);

  placeTerminalRooms(_config.getNumTerminalRoomsMiddle(),
                     _config.getInnerCircleRadius(),
                     _config.getMiddleCircleRadius(), gen);

  placeTerminalRooms(_config.getNumTerminalRoomsOuter(),
                     _config.getMiddleCircleRadius(), _config.getMapRadius(),
                     gen);

  _generator_step = [this]() {
    this->separateRooms([this]() { this->markAndFillHallways(); });
  };
}

void LevelGenerator::placeTerminalRooms(int num_rooms, float min_radius,
                                        float max_radius,
                                        std::default_random_engine &generator) {

  std::uniform_real_distribution<float> dis(0.0f, 1.0f);

  // Make sure the room is always inside of the spawn circle;
  float terminal_radius =
      static_cast<cugl::Vec2>(LevelGenerator::TERMINAL).length() / 2.0f;
  min_radius += terminal_radius;
  max_radius -= terminal_radius;

  float r = dis(generator) * (max_radius - min_radius) + min_radius;
  float min_angle = dis(generator) * 2 * M_PI;
  float max_angle = min_angle + M_PI / 2.0f;

  for (float i = 0; i < num_rooms; i++) {
    Room room(LevelGenerator::TERMINAL);
    room.node->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_LEFT);

    float angle = dis(generator) * (max_angle - min_angle) + min_angle;
    cugl::Vec2 pos(floor(r * cosf(angle)), floor(r * sinf(angle)));

    room.node->setPosition(pos - (room.node->getContentSize() / 2.0f));
    room.node->setColor(cugl::Color4(52, 205, 14, 127));

    min_angle += 2 * M_PI / num_rooms;
    max_angle += 2 * M_PI / num_rooms;

    _rooms.push_back(room);
    _map->addChild(room.node);

    Room &overlapping = roomMostOverlappingWith(room);
    if (overlapping != room) {
      overlapping.node->setColor(cugl::Color4(205, 14, 14, 127));
      auto it = std::find(_rooms.begin(), _rooms.end(), overlapping);
      if (it != _rooms.end()) {
        _map->removeChild(overlapping.node);
        *it = room;
      }
    }
  }
}

void LevelGenerator::markAndFillHallways() {

  std::vector<Room> inside_circle;
  std::copy_if(_rooms.begin(), _rooms.end(), std::back_inserter(inside_circle),
               [this](const Room &room) {
                 return room.getMid().length() <=
                        this->_config.getInnerCircleRadius();
               });

  std::vector<Room> middle_circle;
  std::copy_if(_rooms.begin(), _rooms.end(), std::back_inserter(middle_circle),
               [this](const Room &room) {
                 float r = room.getMid().length();
                 return r > this->_config.getInnerCircleRadius() &&
                        r <= this->_config.getMiddleCircleRadius();
               });

  std::vector<Room> outside_circle;
  std::copy_if(_rooms.begin(), _rooms.end(), std::back_inserter(outside_circle),
               [this](const Room &room) {
                 return room.getMid().length() >
                        this->_config.getMiddleCircleRadius();
               });

  // calculateDelaunayTriangles(inside_circle);
  // calculateDelaunayTriangles(middle_circle);
  calculateDelaunayTriangles(outside_circle, _config.getMiddleCircleRadius());
  // calculateDelaunayTriangles(outside_circle, 0);

  // calculateMinimumSpanningTree();
  // addEdgesBack();
  // fillHallways();

  // _generator_step = [this]() { this->buildCompositeAreas(); };
  _generator_step = nullptr;
}

void LevelGenerator::buildCompositeAreas() {
  _generator_step = [this]() { this->establishGates(); };
}

void LevelGenerator::establishGates() { _generator_step = nullptr; }

void LevelGenerator::calculateDelaunayTriangles(
    std::vector<Room> &rooms, float inner_circle_cutoff_radius) {

  std::vector<double> coords;

  for (Room &room : rooms) {
    cugl::Vec2 mid_point = room.getMid();

    coords.push_back(static_cast<double>(mid_point.x));
    coords.push_back(static_cast<double>(mid_point.y));
  }

  delaunator::Delaunator d(coords);

  for (std::size_t i = 0; i < d.triangles.size(); i += 3) {
    Room &node_0 = rooms[d.triangles[i]];
    Room &node_1 = rooms[d.triangles[i + 1]];
    Room &node_2 = rooms[d.triangles[i + 2]];

    Edge edge_0_1(node_0, node_1);
    if (!edge_0_1.doesIntersect(cugl::Vec2::ZERO, inner_circle_cutoff_radius)) {
      _map->addChild(edge_0_1.path);
      node_0.edges.push_back(edge_0_1);
      node_1.edges.push_back(edge_0_1);
    }

    Edge edge_0_2(node_0, node_2);
    if (!edge_0_2.doesIntersect(cugl::Vec2::ZERO, inner_circle_cutoff_radius)) {
      _map->addChild(edge_0_2.path);
      node_0.edges.push_back(edge_0_2);
      node_2.edges.push_back(edge_0_2);
    }

    Edge edge_1_2(node_1, node_2);
    if (!edge_1_2.doesIntersect(cugl::Vec2::ZERO, inner_circle_cutoff_radius)) {
      _map->addChild(edge_1_2.path);
      node_1.edges.push_back(edge_1_2);
      node_2.edges.push_back(edge_1_2);
    }
  }

  _map->doLayout();
}

void LevelGenerator::calculateMinimumSpanningTree() {
  int num_edges = 0;
  std::vector<Edge> result;
  // Reset tree to start Prim's algorithm.
  for (Room &room : _rooms) {
    room.visited = false;
  }
  _rooms[0].visited = true;

  while (num_edges < _rooms.size() - 1) {
    Edge min_edge;

    for (std::shared_ptr<Room> room : _rooms) {
      if (room->visited) {
        for (std::shared_ptr<Edge> edge : room->edges) {
          if (!edge->neighbor->visited) {
            min_edge = edge;
          }
        }
      }
    }

    num_edges++;

    if (min_edge) {
      min_edge->neighbor->visited = true;
      result.push_back(min_edge);
    }
  }

  for (Room &room : _rooms) {
    for (Edge &edge : room.edges) {
      edge.path->setVisible(false);
    }
  }

  for (std::shared_ptr<Edge> edge : result) {
    edge->path->setColor(cugl::Color4(255, 14, 14, 124));
    edge->path->setVisible(true);
  }
}

void LevelGenerator::addEdgesBack() {}

void LevelGenerator::fillHallways() {}