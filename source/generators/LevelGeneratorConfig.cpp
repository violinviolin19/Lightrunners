#include "LevelGeneratorConfig.h"

namespace level_gen {

LevelGeneratorConfig::LevelGeneratorConfig()
    : _inner_circle_frac(0.4f),
      _middle_circle_frac(0.7f),
      _hallway_radius(1),
      _num_rooms(100),
      _separation_between_layers(15),
      _max_hallway_length(40),
      _add_edges_back_prob(0.2f),
      _max_num_of_edges(4) {
  setMapRadius(120);
  setNumTerminalRooms(7);
}

}  // namespace level_gen