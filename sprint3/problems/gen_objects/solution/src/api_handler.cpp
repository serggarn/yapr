#include "api_handler.h"
#include "system/settings.h"
#include "model/json_tags.h"


namespace api_handler {
void ApiHandler::MapsToStr(std::string& answ) {
	json::array result;
	for (auto const& map : game_.GetMaps() ) {
		json::object js_map;
		auto id = map.GetId();
		js_map[json_tags::id] = *id;
		js_map[json_tags::name] = map.GetName();
		result.push_back(js_map);
	}
	answ = json::serialize(result);
}

std::optional<std::string> ApiHandler::MapToStr(std::string_view map_id) {
	auto id = model::Map::Id{std::string(map_id)};
	auto map = game_.FindMap(id);
	if ( map == nullptr )
		return std::nullopt;
	json::object result;
	result[json_tags::id] = *(map->GetId());
	result[json_tags::name] = map->GetName();
	json::array jsn_roads;
	RoadsToJson(*map, jsn_roads);
	result[json_tags::roads] = jsn_roads;
	json::array jsn_bldgs;

	BuildingsToJson(*map, jsn_bldgs);
	result[json_tags::buildings] = jsn_bldgs;

	json::array jsn_ofcs;
	OfficesToJson(*map, jsn_ofcs);
	result[json_tags::offices] = jsn_ofcs;

    json::array jsn_loots;
    LootDefinitionsToJson(*map, jsn_loots);
	result[json_tags::lootTypes] = jsn_loots;

	return json::serialize(result);
}

void ApiHandler::RoadsToJson(const model::Map& map, json::array& jsn_array) {
	for (auto const& road : map.GetRoads() ) {
		json::object jsn_rd;
		model::Point start = road->GetStart();
		jsn_rd[json_tags::x0] = start.x;
		jsn_rd[json_tags::y0] = start.y;
		if ( road->IsVertical() )
			jsn_rd[json_tags::y1] = road->GetEnd().y;
		else
			jsn_rd[json_tags::x1] = road->GetEnd().x;
		jsn_array.push_back(jsn_rd);
	}
}

void ApiHandler::BuildingsToJson(const model::Map& map, json::array& jsn_array) {
	for (auto const& bldg : map.GetBuildings() ) {
		json::object jsn_bldg;
		auto rct = bldg.GetBounds();
		jsn_bldg[json_tags::x] = rct.position.x;
		jsn_bldg[json_tags::y] = rct.position.y;
		jsn_bldg[json_tags::w] = rct.size.width;
		jsn_bldg[json_tags::h] = rct.size.height;
		jsn_array.push_back(jsn_bldg);
	}	
}

void ApiHandler::OfficesToJson(const model::Map& map, json::array& jsn_array) {
	for (auto const& ofc : map.GetOffices() ) {
		json::object jsn_ofc;
		auto id = ofc.GetId();
		auto pos = ofc.GetPosition();
		auto offset = ofc.GetOffset();
		jsn_ofc[json_tags::id] = *id;
		jsn_ofc[json_tags::x] = pos.x;
		jsn_ofc[json_tags::y] = pos.y;
		jsn_ofc[json_tags::offsetX] = offset.dx;
		jsn_ofc[json_tags::offsetY] = offset.dy;
		jsn_array.push_back(jsn_ofc);
	}	
}

void ApiHandler::LootDefinitionsToJson(const model::Map &map, json::array &jsn_array) {
    for (auto const& loot : map.GetLootsDefinitions() ) {
        jsn_array.push_back(loot.GetDefiinition());
    }
}

void ApiHandler::LootsToJson(const model::Map &map, json::object &jsn_array) {
    size_t index = 0;
    for (auto const& loot : map.GetLoots() ) {
        auto json_dict = json::object();
        auto json_object = json::object();
        auto point = loot.GetPosition();
        json_object[json_tags::pos] = json::array({point.x, point.y});
        json_object[json_tags::type] = loot.GetType();
//        json_dict[std::to_string(index)] = json_object;
//        jsn_array.push_back(json_dict);
        jsn_array[std::to_string(index)] = json_object;
        index++;
    }
}

StringResponse ApiHandler::MakeError(const StringRequest& request, const http::status status,
                         const std::string& code, const std::string& message) {
    json::object answ_obj;
    answ_obj[json_tags::code] = code;
    answ_obj[json_tags::message] = message;
    std::string answer = request.method() == http::verb::head ? "" : json::serialize(answ_obj);
    return MakeStringResponse(status, answer, request.version(), request.keep_alive());
}

StringResponse ApiHandler::MakeUnallowedMethodError(const StringRequest& request, std::string_view allow_method) {
    json::object answ_obj;
    auto status = http::status::method_not_allowed;
    answ_obj[json_tags::code] = "invalidMethod"s;
    std::stringstream smsg;
    smsg << "Only "sv << std::string{allow_method}.c_str() << " method is expected"sv;
    answ_obj[json_tags::message] = smsg.str();
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
        for (const auto &player: playrs) {
            json::object player_json;
            player_json[json_tags::name] = player.second.GetName();
            auto player_id = std::to_string(*(player.second.GetId()));
            answ_obj[player_id] = player_json;
        }
        std::string answer = request.method() == http::verb::head ? "" : json::serialize(answ_obj);
        return MakeStringResponse(status, answer, request.version(), request.keep_alive());
    });
}

json::object ApiHandler::PlayerStateToJson(const Player &player) {
    auto json_player = json::object();
    auto dog = player.GetDog();
    json::array pos_arr;
    json::value x = json::value(dog->GetPos().x).as_double();
    pos_arr.emplace_back(dog->GetPos().x);
    pos_arr.emplace_back(dog->GetPos().y);
    json_player[json_tags::pos] = pos_arr;
    json::array pos_speed;
    pos_speed.emplace_back(dog->GetSpeed().vx);
    pos_speed.emplace_back(dog->GetSpeed().vy);
    json_player[json_tags::speed] = pos_speed;
    json_player[json_tags::dir] = dog->GetDirStr();
    return json_player;
}

StringResponse ApiHandler::GetGameState(const StringRequest& request) {
    return ExecuteAuthorized(request, [this, &request](const Token& token) -> StringResponse {
        json::object answ_obj;
        auto status = http::status::ok;
        auto player = players_.FindByToken(token);
        json::object plyrs_jsn;
        auto playrs = players_.GetPlayers();
        for (const auto &plyr: playrs) {
            auto player_id = std::to_string(*(plyr.second.GetId()));
            plyrs_jsn[player_id] = PlayerStateToJson(plyr.second);
        }
        answ_obj[json_tags::players] = plyrs_jsn;
        if  ( playrs.size() == 0 )
            answ_obj[json_tags::lostObjects] = json::array();
        else {
            auto map = playrs.begin()->second.GetSession()->GetMap();
            auto loots_jsn = json::object();
            LootsToJson(*map, loots_jsn);
            answ_obj[json_tags::lostObjects] = loots_jsn;
        }
        std::string answer = request.method() == http::verb::head ? "" : json::serialize(answ_obj);
        return MakeStringResponse(status, answer, request.version(), request.keep_alive());
    });
}

StringResponse ApiHandler::SetPlayerAction(const StringRequest& request) {
    return ExecuteAuthorized(request, [this, &request] (const Token &token) -> StringResponse {
        auto status = http::status::ok;
        json::object answ_obj;
        auto player = players_.FindByToken(token);
        auto jsn_values = json::parse(request.body());
        auto direction = jsn_values.as_object().at(json_tags::move).as_string();
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
    auto jsn_values = json::parse(request.body());
    auto delta = jsn_values.as_object().at(json_tags::timeDelta).as_int64();
    std::string answer = json::serialize(answ_obj);
    game_.Tick(std::chrono::milliseconds(delta));
    return  MakeStringResponse(status, answer, request.version(), request.keep_alive());
}

bool ApiHandler::CheckTestEnv() {
    return settings::Settings::GetInstance()->IsTestEnv();
}

StringResponse ApiHandler::JoinGameUseCase(const StringRequest& request) {
    StringResponse resp;
    json::object answ_obj;
    switch (request.method()) {
        case http::verb::post: {
            try {
                using gs = model::GameSession;
                auto jsn_values = json::parse(request.body());
                auto userName = jsn_values.as_object().at(json_tags::userName).as_string();
                auto mapId = jsn_values.as_object().at(json_tags::mapId).as_string();
                if (userName.size() == 0) {
                    return MakeNameError(request);
                } else if (game_.FindMap(model::Map::Id{mapId.c_str()}) == nullptr) {
                    return MakeMapNotFoundError(request);
                } else {
                    auto name = userName.c_str();
                    auto map = game_.FindMap(model::Map::Id{mapId.c_str()});
//                    auto dog_id = model::Dog::Id{name};
                    std::shared_ptr<gs> game_session{nullptr};
                    // если уже есть сессия, то найдём её
                    if (game_.FindMap(model::Map::Id{mapId.c_str()}) != nullptr) {
                        for (auto& session: game_.GetSessions()) {
                            if (*(session->GetMap()->GetId()) == mapId) {
                                game_session = session;
                            }
                        }
                    }
                    if (game_session == nullptr) {
                        gs gm_ses{gs::Id{mapId.c_str()}, map,
                                                  game_.GetLootGeneratorConfig()};
                        game_.AddSession(gm_ses);
                        game_session = game_.FindGameSession(gs::Id{mapId.c_str()});
                    }
                    game_session->AddDog(name);
                    auto dg = game_session->FindDog(model::Dog::Id{name});
                    auto token = players_.AddPlayer(dg, game_session);
                    answ_obj[json_tags::authToken] = *(token.second);
                    answ_obj[json_tags::playerId] = *(token.first);
                    std::string answer = serialize(answ_obj);
                    return MakeStringResponse(http::status::ok, answer, request.version(), request.keep_alive());
                }
            }
            catch (...) {
                return MakeJoinError(request);
            }

            break;
        }
        default: {
            return MakeUnallowedMethodError(request, "POST"sv);
            break;
        }
    }
}

StringResponse ApiHandler::PlayersGameUseCase(const StringRequest& request) {
    StringResponse resp;
    switch (request.method()) {
        case http::verb::get:
        case http::verb::head: {
            try {
                return GetPlayers(request);
            } catch (...) {
                return MakeUnauthorizedError(request);
            }
            break;
        }
        default: {
            return MakeUnallowedMethodError(request, "GET, HEAD"sv);
            break;
        }
    }
}

StringResponse ApiHandler::ActionGameUseCase(const StringRequest& request) {
    StringResponse resp;
    switch (request.method()) {
        case http::verb::post: {
            try {
                return SetPlayerAction(request);
            } catch (...) {
                return MakeUnauthorizedError(request);
            }
            break;
        }
        default: {
            return MakeUnallowedMethodError(request, "POST"sv);
            break;
        }
    }
}

StringResponse ApiHandler::StateGameUseCase(const StringRequest& request) {
    StringResponse resp;
    switch (request.method()) {
        case http::verb::get:
        case http::verb::head: {
            try {
                return GetGameState(request);
            } catch (...) {
                return MakeUnauthorizedError(request);
            }
            break;
        }
        default: {
            return MakeUnallowedMethodError(request, "GET, HEAD"sv);
            break;
        }
    }

}

StringResponse ApiHandler::TickGameUseCase(const StringRequest& request) {
    if ( ! CheckTestEnv() )
        return MakeInvalidEndpointError(request);
    else
        switch (request.method()) {
            case http::verb::post: {
                try {
                    return SetTick(request);
                } catch (...) {
                    return MakeTickError(request);
                }
                break;
            }
            default: {
                return MakeUnallowedMethodError(request, "POST"sv);
                break;
            }
        }

}

StringResponse ApiHandler::MapsGameUseCase(const StringRequest& request) {
    std::string answer;
    switch (request.method()) {

        case http::verb::get:
        case http::verb::head: {
            if (request.target() == UriType::URI_MAPS) {
                MapsToStr(answer);
                return MakeStringResponse(http::status::ok, answer, request.version(), request.keep_alive());
            } else {
                auto map_id = request.target().substr(UriType::URI_MAPS.size());
                map_id.remove_prefix(std::min(map_id.find_first_not_of("/"), map_id.size()));
                if (auto mp_str = MapToStr(map_id)) {
                    answer = mp_str.value();
                    return MakeStringResponse(http::status::ok, answer, request.version(), request.keep_alive());
                }
                else {
                    // карту не нашли
                    return MakeMapNotFoundError(request);
                }
            }
            break;
        }
        default: {
            return MakeUnallowedMethodError(request, "GET, HEAD"sv);
            break;
        }
    }

}

} // namespace api_handler

