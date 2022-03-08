#ifndef MODELS_LEVEL_GEN_ROOM_H_
#define MODELS_LEVEL_GEN_ROOM_H_
#include <cugl/cugl.h>

class Edge;

class Room {
 public:
  /** RoomType is used by the level generator to define when the room should be
   * spawned  */
  enum RoomType {
    /** Represents a terminal room, that is spawned in the second separation
       phase of room spawning. */
    TERMINAL,
    /** Represents a spawn room that is spawned in the center of the dungeon.
       Should be used only once. */
    SPAWN,
    /** Represents any standard type of room. */
    STANDARD
  };

  /** This is the room type of the room. */
  RoomType _type;

  /** A reference to the scene2 node for the room. */
  std::shared_ptr<cugl::scene2::PolygonNode> _node;

  /** A list of all edges to other Rooms. */
  std::vector<std::shared_ptr<Edge>> _edges;

  /** A list of all the doors in the Room given by the grid unit coordinates in
   * relation to the bottom left of the room. */
  std::vector<cugl::Vec2> _doors;

  /** If the room has been visited by the level generation algorithm. */
  bool _visited;

  /** If this room should not move. */
  bool _fixed;

  /**
   * Create a Room with given width and height in grid units. The doors will be
   * at in the middle of the four room edges.
   *
   * @param width Width of room in grid units.
   * @param height Height of room in grid units.
   */
  Room(float width, float height) : Room(cugl::Size(width, height)) {}

  /**
   * Create a Room with given size. The doors will be at in the middle of the
   * four room edges.
   *
   * @param size Size of room in grid units.
   */
  Room(cugl::Size size);

  /**
   * Create a Room with given width and height in grid units and doors. The
   * doors are represented by their grid unit coordinates in terlation to the
   * bottom left of the room.
   *
   * @param width Width of room in grid units.
   * @param height Height of room in grid units.
   * @param doors The list of door grid unit coordinates in relation to the
   * bottom left of the room.
   */
  Room(float width, float height, std::vector<cugl::Vec2> doors)
      : Room(cugl::Size(width, height), doors) {}

  /**
   * Create a Room with given size in grid units and doors. The doors are
   * represented by their grid unit coordinates in terlation to the bottom left
   * of the room.
   *
   * @param size Size of room in grid units.
   * @param doors The list of door grid unit coordinates in relation to the
   * bottom left of the room.
   */
  Room(cugl::Size size, std::vector<cugl::Vec2> doors);

  /**
   * A copy constructor for Room that copies the type, doors and size.
   *
   * @param room The room to copy.
   */
  Room(const Room &room) : Room(room._node->getContentSize(), room._doors) {
    _type = room._type;
  }

  /**
   * Move the room by given certain distance.
   *
   * @param dist The distance to move the room in grid units.
   */
  void move(cugl::Vec2 dist);

  /**
   * Find if the given room is a neighbor of this room.
   *
   * @param room The room we would like to check for neighborness.
   * @return If the given room is a neighbor of this room.
   */
  bool hasNeighbor(const std::shared_ptr<Room> &room) const;

  /**
   * Find and return the edge that ties this room and the given room.
   *
   * @param room The room that is connected to this room.
   * @return The edge that connects param room and this room. Returns nullptr if
   * no edges found.
   */
  std::shared_ptr<Edge> findEdge(const std::shared_ptr<Room> &room) const;

  /**
   * Get a rectangle representation of the room.
   * @return cugl::Rect with room's position and size.
   */
  cugl::Rect getRect() const {
    return cugl::Rect(_node->getPosition(), _node->getSize());
  }

  /**
   * Get the midpoint of the room.
   * @return cugl::Vec2 of the midpoint of the room.
   */
  cugl::Vec2 getMid() const {
    return _node->getPosition() + _node->getSize() / 2.0f;
  }

  /**
   * Get the diagonal radius of the room.
   * @return The diagonal radius of the room.
   */
  float getRadius() const {
    return cugl::Vec2(_node->getSize()).length() / 2.0f;
  }

 private:
  /**
   * Initialize the scene2 nodes for this room given a certain size in grid
   * units.
   *
   * @param size The size of the room in grid units.
   */
  void initScene2(cugl::Size size);
};

/**
 * This class represents a connection between two rooms.
 */
class Edge {
 public:
  /** A reference to the source Room. */
  std::shared_ptr<Room> _source;

  /** A reference to the neighbor Room. */
  std::shared_ptr<Room> _neighbor;

  /** A reference to the scene2 node for debug drawing. */
  std::shared_ptr<cugl::scene2::PathNode> _path;

  /** The length of the edge. */
  float _weight;

  /** Used by the level generator to define if the edge has already been
   * calculated during level generation. */
  bool _calculated;

  /**
   * Construct an Edge that connects Rooms s and n.
   * @param s Room for the source of the edge.
   * @param n Room for the neighbor of the edge.
   */
  Edge(const std::shared_ptr<Room> &s, const std::shared_ptr<Room> &n);

  /**
   * Check if the edge intersects a circle with given origin and radius.
   * @param origin The origin of the circle in grid units.
   * @param r The radius of the circle in grid units.
   * @return If the edge intersects the given circle.
   */
  bool doesIntersect(cugl::Vec2 origin, float r);

  /**
   * Get the other Room given one of the rooms in the edge.
   * @param room The room that is in the edge.
   * @return A reference to the other room or nullptr if the given room is not
   * on the edge.
   */
  std::shared_ptr<Room> getOther(const std::shared_ptr<Room> &room) {
    if (room == _source) {
      return _neighbor;
    } else if (room == _neighbor) {
      return _source;
    }
    return nullptr;
  }

  /**
   * An equality operator for Edge that checks if the other Edge has the same
   * rooms in any order of source and neighbor.
   * @param other The other edge to check for equality.
   * @return If the other Edge is equal to this Edge.
   * */
  bool operator==(const Edge &other) const {
    return (this->_source == other._source &&
            this->_neighbor == other._neighbor) ||
           (this->_source == other._neighbor &&
            this->_neighbor == other._source);
  }

  /**
   * Check if the other Edge shares a Room with this Edge.
   * @param other The other edge to check for sharedness.
   * @return If the other Edge shares a room with this Edge.
   */
  bool shareRoom(const std::shared_ptr<Edge> &other) const {
    return this->_source == other->_source ||
           this->_neighbor == other->_source ||
           this->_neighbor == other->_neighbor ||
           this->_source == other->_neighbor;
  }
};

#endif  // MODELS_LEVEL_GEN_ROOM_H_
