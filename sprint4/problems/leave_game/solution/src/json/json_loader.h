#pragma once

#include <filesystem>
#include <boost/json.hpp>
#include "../game_play/game.h"

namespace json_loader {

namespace json = boost::json;
using namespace std::literals;

model::Game LoadGame(const std::filesystem::path& json_path);
std::stringstream LoadFile(const std::filesystem::path& json_path);
model::Map LoadMap(const json::value& jsn_value,
                   const double& default_dog_speed,
                   const size_t& default_bag_capacity,
                   const std::chrono::seconds default_dog_retirement_time);
model::Road LoadRoad(const json::value& jsn_value);
model::Building LoadBuilding(const json::value& jsn_value);
model::Office LoadOffice(const json::value& jsn_value);

}  // namespace json_loader
