#include "LevelGeneratorConfig.h"

LevelGeneratorConfig::LevelGeneratorConfig()
    : _inner_circle_frac(0.4f),
      _middle_circle_frac(0.7f),
      _hallway_radius(1),
      _num_rooms(100),
      _num_separation_iterations(0),
      _separation_between_layers(8),
      _max_hallway_length(20),
      _add_edges_back_prob(0.2f) {
  setMapRadius(60);
  setNumTerminalRooms(7);
}