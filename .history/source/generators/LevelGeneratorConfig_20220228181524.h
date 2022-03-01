#ifndef GENERATORS_LEVEL_GENERATOR_CONFIG_H
#define GENERATORS_LEVEL_GENERATOR_CONFIG_H
#include <cugl/cugl.h>

class LevelGeneratorConfig {
private:
  bool _see_separation;
  float _map_radius;

  float _inner_circle_radius;
  float _inner_circle_frac;

  float _middle_circle_radius;
  float _middle_circle_frac;

  float _hallway_radius;
  float _num_rooms;

  float _num_terminal_rooms;

  float _num_terminal_rooms_inner;
  float _num_terminal_rooms_middle;
  float _num_terminal_rooms_outer;

  int _num_separation_iterations;

public:
  LevelGeneratorConfig();
  ~LevelGeneratorConfig() {}

  void setMapRadius(int radius) {
    _map_radius = static_cast<float>(radius);
    _inner_circle_radius = floor(_map_radius * _inner_circle_frac);
    _middle_circle_radius = floor(_map_radius * _middle_circle_frac);
  }
  float getMapRadius() { return _map_radius; }

  void setInnerCircleFrac(float frac) {
    _inner_circle_frac = frac;
    _inner_circle_radius = floor(_map_radius * frac);
  }
  float getInnerCircleRadius() { return _inner_circle_radius; }
  float getInnerCircleFrac() { return _inner_circle_frac; }

  void setMiddleCircleFrac(float frac) {
    _middle_circle_frac = frac;
    _middle_circle_radius = _map_radius * frac;
  }
  float getMiddleCircleRadius() { return _middle_circle_radius; }
  float getMiddleCircleFrac() { return _middle_circle_frac; }

  void setHallwayRadius(int radius) {
    _hallway_radius = static_cast<float>(radius);
  }

  float getHallwayRadius() { return _hallway_radius; }

  void setNumRooms(int num) { _num_rooms = static_cast<float>(num); }

  float getNumRooms() { return _num_rooms; }

  void setNumTerminalRooms(int num) {
    float num_f = static_cast<float>(num);
    _num_terminal_rooms = num_f;

    // Minimum of dividing by 3. (eg. 7 -> 2)
    _num_terminal_rooms_inner = floor(num_f / 3);
    _num_terminal_rooms_middle = floor(num_f / 3);

    // Majority of dividing by 3. (eg. 7 -> 3)
    _num_terminal_rooms_outer = ceil(num_f / 3);
  }

  float getNumTerminalRooms() { return _num_terminal_rooms; }
  float getNumTerminalRoomsInner() { return _num_terminal_rooms_inner; }
  float getNumTerminalRoomsMiddle() { return _num_terminal_rooms_middle; }
  float getNumTerminalRoomsOuter() { return _num_terminal_rooms_outer; }
};

#endif /* GENERATORS_LEVEL_GENERATOR_CONFIG_H */
