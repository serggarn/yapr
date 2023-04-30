#include "api_handler.h"

namespace api_handler {
void ApiHandler::MapsToStr(std::string& answ) {
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

int ApiHandler::MapToStr(std::string_view map_id, std::string& answ) {
	auto id = model::Map::Id{std::string(map_id)};
	auto map = game_.FindMap(id);
	if ( map == nullptr )
		return 1;
	json::object result;
	result["id"] = *(map->GetId());
	result["name"] = map->GetName();
	json::array jsn_roads;
	RoadsToJson(*map, jsn_roads);
	result["roads"] = jsn_roads;
	json::array jsn_bldgs;
	
	BuildingsToJson(*map, jsn_bldgs);
	result["buildings"] = jsn_bldgs;

	json::array jsn_ofcs;
	OfficesToJson(*map, jsn_ofcs);
	result["offices"] = jsn_ofcs;
	answ = json::serialize(result);
	return 0;
}

void ApiHandler::RoadsToJson(const model::Map& map, json::array& jsn_array) {
	for (auto const& road : map.GetRoads() ) {
		json::object jsn_rd;
		model::Point start = road->GetStart();
		jsn_rd["x0"] = start.x;
		jsn_rd["y0"] = start.y;
		if ( road->IsVertical() )
			jsn_rd["y1"] = road->GetEnd().y;
		else
			jsn_rd["x1"] = road->GetEnd().x;
		jsn_array.push_back(jsn_rd);
	}
}

void ApiHandler::BuildingsToJson(const model::Map& map, json::array& jsn_array) {
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

void ApiHandler::OfficesToJson(const model::Map& map, json::array& jsn_array) {
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
StringResponse ApiHandler::MakeUnauthorizedError(const StringRequest& request) {
    json::object answ_obj;
    auto status = http::status::unauthorized;
    answ_obj["code"] = "invalidToken"s;
    answ_obj["message"] = "Authorization header is missing"s;
    std::string answer = request.method() == http::verb::head ? "" : json::serialize(answ_obj);
    return MakeStringResponse(status, answer, request.version(), request.keep_alive());
}

StringResponse ApiHandler::MakeUnknownTokenError(const StringRequest& request) {
    json::object answ_obj;
    auto status = http::status::unauthorized;
    answ_obj["code"] = "unknownToken"s;
    answ_obj["message"] = "Player token has not been found"s;
    std::string answer = request.method() == http::verb::head ? "" : json::serialize(answ_obj);
    return MakeStringResponse(status, answer, request.version(), request.keep_alive());
}

StringResponse ApiHandler::MakeTickError(const StringRequest &request) {
    json::object answ_obj;
    auto status = http::status::bad_request;
    answ_obj["code"] = "invalidArgument"s;
    answ_obj["message"] = "Failed to parse tick request JSON"s;
    std::string answer = json::serialize(answ_obj);
    return MakeStringResponse(status, answer, request.version(), request.keep_alive());
}

StringResponse ApiHandler::MakeUnallowedMethodError(const StringRequest& request, std::string_view allow_method) {
    json::object answ_obj;
    auto status = http::status::method_not_allowed;
    answ_obj["code"] = "invalidMethod"s;
    std::stringstream smsg;
    smsg << "Only "sv << std::string{allow_method}.c_str() << " method is expected"sv;
    answ_obj["message"] = smsg.str();
    std::string answer = request.method() == http::verb::head ? "" : json::serialize(answ_obj);//answer.c_str();
    return MakeStringResponse(status, answer, request.version(), request.keep_alive(),
                              ContentType::APPL_JSON, allow_method);
}

std::optional<Token> ApiHandler::TryExtractToken(const StringRequest& request) {
    auto header = request.base();
    auto auth = header.at(http::field::authorization);
    if (!auth.starts_with("Bearer") || auth.size() < (32 + 7)) {
        return std::nullopt;
    }
    auto token_str = std::string{auth.substr(auth.find_first_of(" ") + 1)};
    return Token {token_str};
}

bool ApiHandler::AllowToken(const Token& token) {
    auto player = players_.FindByToken(token);
    return player != nullptr;
}

StringResponse ApiHandler::GetPlayers(const StringRequest& request) {
    return ExecuteAuthorized(request, [this, &request](const Token& token) -> StringResponse {
        json::object answ_obj;
        auto status = http::status::ok;
        auto player = players_.FindByToken(token);
        auto playrs = players_.GetPlayers();
        for (const auto &plyr: playrs) {
            json::object plyr_json;
            plyr_json["name"] = plyr.second.GetName();
            answ_obj[std::to_string(*(plyr.second.GetId()))] = plyr_json;
        }
        std::string answer = request.method() == http::verb::head ? "" : json::serialize(answ_obj);
        return MakeStringResponse(status, answer, request.version(), request.keep_alive());
    });
}

StringResponse ApiHandler::GetGameState(const StringRequest& request) {
    return ExecuteAuthorized(request, [this, &request](const Token& token) -> StringResponse {
        json::object answ_obj;
        auto status = http::status::ok;
        auto player = players_.FindByToken(token);
        json::object plyrs_jsn;
        auto playrs = players_.GetPlayers();
        for (const auto &plyr: playrs) {
            json::object plyr_json;
            auto dog = plyr.second.GetDog();
            json::array pos_arr;
            json::value x = json::value(dog->GetPos().x).as_double();
            pos_arr.emplace_back(dog->GetPos().x);
            pos_arr.emplace_back(dog->GetPos().y);
            plyr_json["pos"] = pos_arr;
            json::array pos_speed;
            pos_speed.emplace_back(dog->GetSpeed().vx);
            pos_speed.emplace_back(dog->GetSpeed().vy);
            plyr_json["speed"] = pos_speed;
            plyr_json["dir"] = dog->GetDirStr();
            plyrs_jsn[std::to_string(*(plyr.second.GetId()))] = plyr_json;
        }
        answ_obj["players"]=plyrs_jsn;
        std::string answer = request.method() == http::verb::head ? "" : json::serialize(answ_obj);
        return MakeStringResponse(status, answer, request.version(), request.keep_alive());
    });
}

StringResponse ApiHandler::SetPlayerAction(const StringRequest& request) {
    return ExecuteAuthorized(request, [this, &request](const Token &token) -> StringResponse {
        auto status = http::status::ok;
        json::object answ_obj;
        auto player = players_.FindByToken(token);
        auto jsn_values = json::parse(request.body());
        auto direction = jsn_values.as_object().at("move").as_string();
        auto dog = player->GetDog();
        auto gs = player->GetSession();
        auto speed = gs->GetMap()->GetDogSpeed();
        dog->SetMove(speed, direction.c_str());
        std::string answer = json::serialize(answ_obj);
        return  MakeStringResponse(status, answer, request.version(), request.keep_alive());
    });
}

StringResponse ApiHandler::SetTick(const StringRequest& request) {
    auto status = http::status::ok;
    json::object answ_obj;
    std::cout <<"11"<<std::endl;
    auto jsn_values = json::parse(request.body());
    std::cout <<"122"<<std::endl;
    auto delta = jsn_values.as_object().at("timeDelta").as_int64();
    std::cout <<"12"<<std::endl;
    std::string answer = json::serialize(answ_obj);
    for (const auto& session : game_.GetSessions()) {
        auto gs = game_.FindGameSession(session->GetId());
        gs->Tick(delta);
    }
    return  MakeStringResponse(status, answer, request.version(), request.keep_alive());
}

} // namespace api_handler

