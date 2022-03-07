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
const RoomConfig kTerminal = {
    cugl::Size(11.0f, 11.0f),
    std::vector<cugl::Vec2>{cugl::Vec2(0.0f, 5.0f), cugl::Vec2(5.0f, 10.0f),
                            cugl::Vec2(10.0f, 5.0f), cugl::Vec2(5.0f, 0.0f)}};

/** Represents the default spawn room size and doors. */
const RoomConfig kSpawn = {
    cugl::Size(11.0f, 11.0f),
    std::vector<cugl::Vec2>{cugl::Vec2(0.0f, 5.0f), cugl::Vec2(5.0f, 10.0f),
                            cugl::Vec2(10.0f, 5.0f), cugl::Vec2(5.0f, 0.0f)}};

/** Represents a standard room. */
const RoomConfig kStandard1 = {
    cugl::Size(11.0f, 11.0f),
    std::vector<cugl::Vec2>{cugl::Vec2(0.0f, 3.0f), cugl::Vec2(7.0f, 10.0f),
                            cugl::Vec2(10.0f, 5.0f), cugl::Vec2(5.0f, 0.0f)}};

/** Represents a standard room. */
const RoomConfig kStandard2 = {
    cugl::Size(11.0f, 11.0f),
    std::vector<cugl::Vec2>{cugl::Vec2(0.0f, 5.0f), cugl::Vec2(5.0f, 10.0f),
                            cugl::Vec2(10.0f, 5.0f), cugl::Vec2(5.0f, 0.0f)}};

/** RRepresents a standard rooms. */
const RoomConfig kStandard3 = {
    cugl::Size(11.0f, 11.0f),
    std::vector<cugl::Vec2>{cugl::Vec2(0.0f, 7.0f), cugl::Vec2(2.0f, 10.0f),
                            cugl::Vec2(10.0f, 2.0f), cugl::Vec2(7.0f, 0.0f)}};

/** A list of all the regular rooms. */
const std::vector<RoomConfig> kRegularRooms{kStandard1, kStandard2, kStandard3};

};  // namespace default_rooms

#endif  // MODELS_LEVEL_GEN_DEFAULT_ROOMS_H_