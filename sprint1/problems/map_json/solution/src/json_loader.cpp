#include "json_loader.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace json_loader {

model::Game LoadGame(const std::filesystem::path& json_path) {
    // Загрузить содержимое файла json_path, например, в виде строки
	
	if ( ! std::filesystem::exists(json_path) )
		throw std::runtime_error("File not exist!");;
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
// 		std::cout << "map:" << jsn_id <<std::endl;
		
// 		std::cout << "map:" << json::serialize(jsn_id) <<std::endl;
		model::Map map (model::Map::Id{jsn_id}, jsn_name);
		
		// Добавим дороги
		auto roads = jsn_map.as_object().at("roads").as_array();
		for ( auto const& jsn_road : roads) {
			model::Point start { jsn_road.as_object().at("x0").as_int64(), jsn_road.as_object().at("y0").as_int64() };
			if ( jsn_road.as_object().if_contains("x1")) {
				model::Coord x1 = jsn_road.as_object().at("x1").as_int64();
				model::Road road { model::Road::HORIZONTAL, start, x1 };			
				map.AddRoad(road);
			} else {
				model::Coord y1 = jsn_road.as_object().at("y1").as_int64();
				model::Road road { model::Road::VERTICAL, start, y1 };
				map.AddRoad(road);
			}

		}

		// Добавим здания
		auto jsn_bldngs = jsn_map.as_object().at("buildings").as_array();
		for ( auto const& jsn_bldg : jsn_bldngs ) {
			model::Point point { jsn_bldg.as_object().at("x").as_int64(), jsn_bldg.as_object().at("y").as_int64() };
			model::Size size { jsn_bldg.as_object().at("w").as_int64(),  jsn_bldg.as_object().at("h").as_int64() };
			map.AddBuilding(model::Building {model::Rectangle { point, size }});
		}
		// Добавим Офисы
		auto jsn_ofcs = jsn_map.as_object().at("offices").as_array();
		for ( auto const& jsn_ofc : jsn_ofcs ) {
			auto jsn_id = jsn_ofc.as_object().at("id").as_string().c_str();
			model::Office::Id id { jsn_id };
			model::Point position { jsn_ofc.as_object().at("x").as_int64(), jsn_ofc.as_object().at("y").as_int64() };
			model::Offset offset { jsn_ofc.as_object().at("offsetX").as_int64(), jsn_ofc.as_object().at("offsetY").as_int64() };
			map.AddOffice(model::Office {id, position, offset});
		}
		game.AddMap(map);
	}
    return game;
}

}  // namespace json_loader
