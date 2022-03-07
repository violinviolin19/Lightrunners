// THIS IS A TEMPORARY FILE THAT SHOULD BE REPLACED USING JSON LOADING.

#ifndef MODELS_LEVEL_GEN_DEFAULT_ROOMS_H_
#define MODELS_LEVEL_GEN_DEFAULT_ROOMS_H_

#include "Room.h"

namespace default_rooms {

/** A basic structure that represents a room. */
struct RoomConfig {
  /** The size of the room. */
  cugl::Size size;
  /** The coordinates of the doors in the room. */
  std::vector<cugl::Vec2> doors;
};

/** Represents the default terminal room size and doors. */
const RoomConfig kTerminal = {cugl::Size(10.0f, 10.0f),
                              std::vector<cugl::Vec2>()};

/** Represents the default spawn room size and doors. */
const RoomConfig kSpawn = {cugl::Size(10.0f, 10.0f), std::vector<cugl::Vec2>()};

/** Represents a room with 4 doors. */
const RoomConfig k4Doors = {
    cugl::Size(10.0f, 10.0f),
    std::vector<cugl::Vec2>{cugl::Vec2(0.0f, 3.0f), cugl::Vec2(4.0f, 0.0f),
                            cugl::Vec2(4.0f, 5.0f), cugl::Vec2(7.0f, 3.0f)}};

/** Represents a room with 3 doors. */
const RoomConfig k3Doors = {
    cugl::Size(10.0f, 10.0f),
    std::vector<cugl::Vec2>{cugl::Vec2(4.0f, 0.0f), cugl::Vec2(4.0f, 5.0f),
                            cugl::Vec2(7.0f, 3.0f)}};

/** Represents a room with 2 doors. */
const RoomConfig k2Doors = {
    cugl::Size(10.0f, 10.0f),
    std::vector<cugl::Vec2>{cugl::Vec2(4.0f, 0.0f), cugl::Vec2(4.0f, 5.0f)}};

/** A list of all the regular rooms. */
const std::vector<RoomConfig> kRegularRooms{k4Doors, k3Doors, k2Doors};

};  // namespace default_rooms

#endif  // MODELS_LEVEL_GEN_DEFAULT_ROOMS_H_