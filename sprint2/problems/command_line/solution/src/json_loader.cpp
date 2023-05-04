#include "json_loader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "model/json_tags.h"

namespace json_loader {

model::Game LoadGame(const std::filesystem::path& json_path) {
    // Загрузить содержимое файла json_path, например, в виде строки
	auto input_str = LoadFile(json_path);

    // Распарсить строку как JSON, используя boost::json::parse
	auto jsn_values = json::parse(input_str.str());
    // Загрузить модель игры из файла
    model::Game game;
    auto default_dog_speed = jsn_values.as_object().contains("defaultDogSpeed") ?
            jsn_values.as_object().at(json_tags::defaultDogSpeed).as_double() : 1.0;

	auto jsn_maps = jsn_values.as_object().at(json_tags::maps).as_array();
	for ( auto const& jsn_map : jsn_maps ) {
		auto map = LoadMap(jsn_map, default_dog_speed);
		
		// Добавим дороги
		auto roads = jsn_map.as_object().at(json_tags::roads).as_array();
		for ( auto const& jsn_road : roads) {
			map.AddRoad(LoadRoad(jsn_road));
		}

		// Добавим здания
		auto jsn_bldngs = jsn_map.as_object().at(json_tags::buildings).as_array();
		for ( auto const& jsn_bldg : jsn_bldngs ) {
			map.AddBuilding(LoadBuilding(jsn_bldg));
		}

		// Добавим Офисы
		auto jsn_ofcs = jsn_map.as_object().at(json_tags::offices).as_array();
		for ( auto const& jsn_ofc : jsn_ofcs ) {
			map.AddOffice(LoadOffice(jsn_ofc));
		}
//        map.PrintVHRoads();
		game.AddMap(map);
	}
    return game;
}

std::stringstream LoadFile(const std::filesystem::path& json_path) {
	std::stringstream buf;
	if ( ! std::filesystem::exists(json_path) )
		throw std::runtime_error("File not exist!");
	std::ifstream ifs {json_path.string().c_str()};
	buf << ifs.rdbuf();
	ifs.close();	
	return buf;
}

model::Map LoadMap(const json::value& jsn_value, const double& default_dog_speed) {
    auto dg_spd = jsn_value.as_object().contains("dogSpeed") ?
                      jsn_value.as_object().at(json_tags::dogSpeed).as_double() : default_dog_speed;
	auto jsn_id = jsn_value.as_object().at(json_tags::id).as_string().c_str();
	auto jsn_name = jsn_value.as_object().at(json_tags::name).as_string().c_str();
	return { model::Map{ model::Map::Id{jsn_id}, jsn_name, dg_spd } };
}

model::Road LoadRoad(const json::value& jsn_value) {
	auto x0 = json::value_to<model::Coord>(jsn_value.as_object().at(json_tags::x0));
	auto y0 = json::value_to<model::Coord>(jsn_value.as_object().at(json_tags::y0));
	model::Point start { x0, y0 };
	if ( jsn_value.as_object().if_contains(json_tags::x1)) {
		auto x1 = json::value_to<model::Coord>(jsn_value.as_object().at(json_tags::x1));
		return model::Road { model::Road::HORIZONTAL, start, x1 };			
	} else {
		auto y1 = json::value_to<model::Coord>(jsn_value.as_object().at(json_tags::y1));
		return model::Road { model::Road::VERTICAL, start, y1 };
	}
}

model::Building LoadBuilding(const json::value& jsn_value) {
	auto x = json::value_to<model::Coord>(jsn_value.as_object().at(json_tags::x));
	auto y = json::value_to<model::Coord>(jsn_value.as_object().at(json_tags::y));
	
	model::Point point { x, y };
	
	auto w = json::value_to<model::Dimension>(jsn_value.as_object().at(json_tags::w));
	auto h = json::value_to<model::Dimension>(jsn_value.as_object().at(json_tags::h));
	model::Size size { w, h};
	return model::Building { model::Rectangle {point, size} };

}

model::Office LoadOffice(const json::value& jsn_value) {
	auto jsn_id = jsn_value.as_object().at(json_tags::id).as_string().c_str();
	model::Office::Id id { jsn_id };
	auto x = json::value_to<model::Coord>(jsn_value.as_object().at(json_tags::x));
	auto y = json::value_to<model::Coord>(jsn_value.as_object().at(json_tags::y));
	model::Point position { x, y };

	auto ofsX = json::value_to<model::Dimension>(jsn_value.as_object().at(json_tags::offsetX));
	auto ofsY = json::value_to<model::Dimension>(jsn_value.as_object().at(json_tags::offsetY));
	model::Offset offset { ofsX, ofsY };
	return model::Office {id, position, offset};	
}


}  // namespace json_loader
