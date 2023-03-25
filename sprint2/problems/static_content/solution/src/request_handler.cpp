#include "request_handler.h"
#include "model.h"
#include <boost/json.hpp>
// #include <boost/system>

namespace json = boost::json;
namespace sys = boost::system;
namespace beast = boost::beast;
namespace http = beast::http;


namespace http_handler {

void RequestHandler::MapsToStr(std::string& answ) {
	json::array result;
	for (auto const& map : game_.GetMaps() ) {
		json::object js_map;
		auto id = map.GetId();
		js_map["id"] = *id;
		js_map["name"] = map.GetName();
		result.push_back(js_map);
	}
	answ = json::serialize(result);
}

int RequestHandler::MapToStr(std::string_view map_id, std::string& answ) {
	auto id = model::Map::Id{std::string(map_id)};
	auto map = game_.FindMap(id);
	if ( map == nullptr )
		return 1;
	json::object result;
	result["id"] = *(map->GetId());
	result["name"] = map->GetName();
	json::array jsn_roads;
	GetRoads(*map, jsn_roads);
	result["roads"] = jsn_roads;
	json::array jsn_bldgs;
	
	GetBuildings(*map, jsn_bldgs);
	result["buildings"] = jsn_bldgs;

	json::array jsn_ofcs;
	GetOffices(*map, jsn_ofcs);
	result["offices"] = jsn_ofcs;
	answ = json::serialize(result);
	return 0;
}

void RequestHandler::GetRoads(const model::Map& map, json::array& jsn_array) {
	for (auto const& road : map.GetRoads() ) {
		json::object jsn_rd;
		model::Point start = road.GetStart();
		jsn_rd["x0"] = start.x;
		jsn_rd["y0"] = start.y;
		if ( road.IsVertical() ) 
			jsn_rd["y1"] = road.GetEnd().y;
		else
			jsn_rd["x1"] = road.GetEnd().x;
		jsn_array.push_back(jsn_rd);
	}
}

void RequestHandler::GetBuildings(const model::Map& map, json::array& jsn_array) {
	for (auto const& bldg : map.GetBuildings() ) {
		json::object jsn_bldg;
		auto rct = bldg.GetBounds();
		jsn_bldg["x"] = rct.position.x;
		jsn_bldg["y"] = rct.position.y;
		jsn_bldg["w"] = rct.size.width;
		jsn_bldg["h"] = rct.size.height;
		jsn_array.push_back(jsn_bldg);
	}	
}

void RequestHandler::GetOffices(const model::Map& map, json::array& jsn_array) {
	for (auto const& ofc : map.GetOffices() ) {
		json::object jsn_ofc;
		auto id = ofc.GetId();
		auto pos = ofc.GetPosition();
		auto offset = ofc.GetOffset();
		jsn_ofc["id"] = *id;
		jsn_ofc["x"] = pos.x;
		jsn_ofc["y"] = pos.y;
		jsn_ofc["offsetX"] = offset.dx;
		jsn_ofc["offsetY"] = offset.dy;
		jsn_array.push_back(jsn_ofc);
	}	
}
}  // namespace http_handler