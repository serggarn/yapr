#include "json_loader.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace json_loader {

model::Game LoadGame(const std::filesystem::path& json_path) {
    // Загрузить содержимое файла json_path, например, в виде строки
	
	if ( ! std::filesystem::exists(json_path) )
		throw std::runtime_error("File not exist!");
	std::stringstream input_str;
	std::ifstream ifs {json_path.string().c_str()};
	input_str << ifs.rdbuf();
	ifs.close();
    // Распарсить строку как JSON, используя boost::json::parse
	auto jsn_values = json::parse(input_str.str());
    // Загрузить модель игры из файла
    model::Game game;
	auto jsn_maps = jsn_values.as_object()["maps"].as_array();
	for ( auto const& jsn_map : jsn_maps ) {
		auto jsn_id = jsn_map.as_object().at("id").as_string().c_str();
		auto jsn_name = jsn_map.as_object().at("name").as_string().c_str();
		model::Map map (model::Map::Id{jsn_id}, jsn_name);
		
		// Добавим дороги
		auto roads = jsn_map.as_object().at("roads").as_array();
		for ( auto const& jsn_road : roads) {
			auto x0 = json::value_to<model::Coord>(jsn_road.as_object().at("x0"));
			auto y0 = json::value_to<model::Coord>(jsn_road.as_object().at("y0"));
			
			model::Point start { x0, y0 };
			if ( jsn_road.as_object().if_contains("x1")) {
				auto x1 = json::value_to<model::Coord>(jsn_road.as_object().at("x1"));
				model::Road road { model::Road::HORIZONTAL, start, x1 };			
				map.AddRoad(road);
			} else {
				auto y1 = json::value_to<model::Coord>(jsn_road.as_object().at("y1"));
				model::Road road { model::Road::VERTICAL, start, y1 };
				map.AddRoad(road);
			}

		}

		// Добавим здания
		auto jsn_bldngs = jsn_map.as_object().at("buildings").as_array();
		for ( auto const& jsn_bldg : jsn_bldngs ) {
			auto x = json::value_to<model::Coord>(jsn_bldg.as_object().at("x"));
			auto y = json::value_to<model::Coord>(jsn_bldg.as_object().at("y"));
			
			model::Point point { x, y };
			
			auto w = json::value_to<model::Dimension>(jsn_bldg.as_object().at("w"));
			auto h = json::value_to<model::Dimension>(jsn_bldg.as_object().at("h"));
			model::Size size { w, h};
			map.AddBuilding(model::Building {model::Rectangle { point, size }});
		}

		// Добавим Офисы
		auto jsn_ofcs = jsn_map.as_object().at("offices").as_array();
		for ( auto const& jsn_ofc : jsn_ofcs ) {
			auto jsn_id = jsn_ofc.as_object().at("id").as_string().c_str();
			model::Office::Id id { jsn_id };
			auto x = json::value_to<model::Coord>(jsn_ofc.as_object().at("x"));
			auto y = json::value_to<model::Coord>(jsn_ofc.as_object().at("y"));
			model::Point position { x, y };

			auto ofsX = json::value_to<model::Dimension>(jsn_ofc.as_object().at("offsetX"));
			auto ofsY = json::value_to<model::Dimension>(jsn_ofc.as_object().at("offsetY"));
			model::Offset offset { ofsX, ofsY };
			map.AddOffice(model::Office {id, position, offset});
		}
		game.AddMap(map);
	}
    return game;
}

}  // namespace json_loader
