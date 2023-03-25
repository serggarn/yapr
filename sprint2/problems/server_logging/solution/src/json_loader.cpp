#include "json_loader.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace json_loader {

model::Game LoadGame(const std::filesystem::path& json_path) {
    // Загрузить содержимое файла json_path, например, в виде строки
	auto input_str = LoadFile(json_path);

    // Распарсить строку как JSON, используя boost::json::parse
	auto jsn_values = json::parse(input_str.str());
    // Загрузить модель игры из файла
    model::Game game;
	auto jsn_maps = jsn_values.as_object()["maps"].as_array();
	for ( auto const& jsn_map : jsn_maps ) {
		auto map = LoadMap(jsn_map);
		
		// Добавим дороги
		auto roads = jsn_map.as_object().at("roads").as_array();
		for ( auto const& jsn_road : roads) {
			map.AddRoad(LoadRoad(jsn_road));
		}

		// Добавим здания
		auto jsn_bldngs = jsn_map.as_object().at("buildings").as_array();
		for ( auto const& jsn_bldg : jsn_bldngs ) {
			map.AddBuilding(LoadBuilding(jsn_bldg));
		}

		// Добавим Офисы
		auto jsn_ofcs = jsn_map.as_object().at("offices").as_array();
		for ( auto const& jsn_ofc : jsn_ofcs ) {
			map.AddOffice(LoadOffice(jsn_ofc));
		}
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

model::Map LoadMap(const json::value& jsn_value) {
	auto jsn_id = jsn_value.as_object().at("id").as_string().c_str();
	auto jsn_name = jsn_value.as_object().at("name").as_string().c_str();
	return { model::Map{ model::Map::Id{jsn_id}, jsn_name } };
}

model::Road LoadRoad(const json::value& jsn_value) {
	auto x0 = json::value_to<model::Coord>(jsn_value.as_object().at("x0"));
	auto y0 = json::value_to<model::Coord>(jsn_value.as_object().at("y0"));
	model::Point start { x0, y0 };
	if ( jsn_value.as_object().if_contains("x1")) {
		auto x1 = json::value_to<model::Coord>(jsn_value.as_object().at("x1"));
		return model::Road { model::Road::HORIZONTAL, start, x1 };			
	} else {
		auto y1 = json::value_to<model::Coord>(jsn_value.as_object().at("y1"));
		return model::Road { model::Road::VERTICAL, start, y1 };
	}
}

model::Building LoadBuilding(const json::value& jsn_value) {
	auto x = json::value_to<model::Coord>(jsn_value.as_object().at("x"));
	auto y = json::value_to<model::Coord>(jsn_value.as_object().at("y"));
	
	model::Point point { x, y };
	
	auto w = json::value_to<model::Dimension>(jsn_value.as_object().at("w"));
	auto h = json::value_to<model::Dimension>(jsn_value.as_object().at("h"));
	model::Size size { w, h};
	return model::Building { model::Rectangle {point, size} };

}

model::Office LoadOffice(const json::value& jsn_value) {
	auto jsn_id = jsn_value.as_object().at("id").as_string().c_str();
	model::Office::Id id { jsn_id };
	auto x = json::value_to<model::Coord>(jsn_value.as_object().at("x"));
	auto y = json::value_to<model::Coord>(jsn_value.as_object().at("y"));
	model::Point position { x, y };

	auto ofsX = json::value_to<model::Dimension>(jsn_value.as_object().at("offsetX"));
	auto ofsY = json::value_to<model::Dimension>(jsn_value.as_object().at("offsetY"));
	model::Offset offset { ofsX, ofsY };
	return model::Office {id, position, offset};	
}


}  // namespace json_loader
