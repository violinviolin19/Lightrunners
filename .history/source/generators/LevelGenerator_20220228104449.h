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

  Room(cugl::Size size) : visited(false), fixed(false) {
    node = cugl::scene2::PolygonNode::alloc();
    node->setContentSize(size);
  }

  void move(cugl::Vec2 dir) {
    if (!fixed) {
      dir.set(roundf(dir.x), roundf(dir.y));
      node->setPosition(node->getPosition() + dir);
    }
  }

  cugl::Rect getRect() {
    cugl::Rect rect(node->getPosition(), node->getSize());
    return rect;
  }

  cugl::Vec2 getMid() {
    cugl::Rect rect = getRect();
    cugl::Vec2 mid_point(rect.getMidX(), rect.getMidY());
    return mid_point;
  }

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

  bool doesIntersect(cugl::Vec2 origin, float radius) {

    cugl::Vec2 d = neighbor->getMid() - source->getMid();
    cugl::Vec2 f = source->getMid() - origin;

    float a = d.Dot(d);
    float b = 2 * f.Dot(d);
    float c = f.Dot(f) - r * r;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
      // no intersection
    } else {
      // ray didn't totally miss sphere,
      // so there is a solution to
      // the equation.

      discriminant = sqrt(discriminant);

      // either solution may be on or off the ray so need to test both
      // t1 is always the smaller value, because BOTH discriminant and
      // a are nonnegative.
      float t1 = (-b - discriminant) / (2 * a);
      float t2 = (-b + discriminant) / (2 * a);

      // 3x HIT cases:
      //          -o->             --|-->  |            |  --|->
      // Impale(t1 hit,t2 hit), Poke(t1 hit,t2>1), ExitWound(t1<0, t2 hit),

      // 3x MISS cases:
      //       ->  o                     o ->              | -> |
      // FallShort (t1>1,t2>1), Past (t1<0,t2<0), CompletelyInside(t1<0, t2>1)

      if (t1 >= 0 && t1 <= 1) {
        // t1 is the intersection, and it's closer than t2
        // (since t1 uses -b - discriminant)
        // Impale, Poke
        return true;
      }

      // here t1 didn't intersect so we are either started
      // inside the sphere or completely past it
      if (t2 >= 0 && t2 <= 1) {
        // ExitWound
        return true;
      }

      // no intn: FallShort, Past, CompletelyInside
      return
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
