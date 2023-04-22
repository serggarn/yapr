#pragma once
#include "response.h"
#include "game.h"
#include "players.h"
#include <boost/json.hpp>
#include "http_types.h"
#include <sstream>

namespace api_handler {
namespace beast = boost::beast;
namespace http = beast::http;
namespace sys = boost::system;

using namespace http_types;
namespace json = boost::json;
using HandlerResult = std::pair<std::string_view, http::response<http::string_body>>;

class ApiHandler {
private:
	void RoadsToJson(const model::Map& map, json::array& jsn_array);
	void BuildingsToJson(const model::Map& map, json::array& jsn_array);
	void OfficesToJson(const model::Map& map, json::array& jsn_array);	
	void MapsToStr(std::string& answ);
	int MapToStr(std::string_view map_id, std::string& answ);
public:
	ApiHandler (model::Game& game, player::Players& players) : game_{game}, players_{players} {}
	template <typename Body, typename Allocator>
    HandlerResult GetResponse(http::request<Body, http::basic_fields<Allocator>>&& req) {
		http::status status;
		std::string_view body;
		requests_e rqs {OTHER};
		std::string answer;
		std::string_view trg = req.target();
		std::string_view allow_method;
        auto content_type = ContentType::APPL_JSON;

		if ( trg == UriType::URI_JOIN ) {
            json::object answ_obj;
			switch (req.method()) 
			{
				case http::verb::post:
				{
					try {
                        auto jsn_values = json::parse(req.body());
						auto userName = jsn_values.as_object()["userName"].as_string();
						auto mapId = jsn_values.as_object()["mapId"].as_string();
						if ( userName.size() == 0 ) {
							status = http::status::bad_request;
							answ_obj["code"] = "invalidArgument";
							answ_obj["message"] = "Invalid name";
						} else if (game_.FindMap(model::Map::Id{mapId.c_str()}) == nullptr) {
							status = http::status::not_found;
							answ_obj["code"] = "mapNotFound";
							answ_obj["message"] = "Map not found";
						} else {
							status = http::status::ok;
                            auto name = userName.c_str();
							auto map = game_.FindMap(model::Map::Id{mapId.c_str()});
							auto dog_id = model::Dog::Id{name};
// 							player::Dog dog {player::Dog::Id{userName}, userName };
                            std::shared_ptr<model::GameSession> gs;
                            // если уже есть сессия, то найдём её
                            if ( game_.FindMap(model::Map::Id {mapId.c_str()}) != nullptr )
                                for ( const auto& session : game_.GetSessions() ) {
                                if ( *(session.GetMap()->GetId()) == mapId )
                                    gs = std::make_shared<model::GameSession>(session);
                            }
                            if ( gs == nullptr ) {
                                model::GameSession gm_ses{model::GameSession::Id{mapId.c_str()}, map};
                                game_.AddSession(gm_ses);
                                gs = game_.FindGameSession(model::GameSession::Id{mapId.c_str()});
                            }
                            gs->AddDog(name);
							auto dg = gs->FindDog(model::Dog::Id{name});
							auto token = players_.AddPlayer(dg, gs);
							answ_obj["authToken"] = *(token.second);
							answ_obj["playerId"] = *(token.first);
						}
					}
					catch (...) {
						status = http::status::bad_request;
						answ_obj["code"] = "invalidArgument"s;
						answ_obj["message"] = "Join game request parse error"s;
					}

					break;
				}
				default: {
					status = http::status::method_not_allowed;
					answ_obj["code"] = "invalidMethod"s;
					answ_obj["message"] = "Only POST method is expected"s;
					allow_method = "POST"sv;
					break;
				}
			}
            answer = json::serialize(answ_obj);
            body = answer;
			auto resp =  MakeStringResponse(status, body, req.version(), req.keep_alive(),
                                            rqs, content_type, allow_method);
			
			HandlerResult res = std::make_pair( content_type, std::move(resp) );
			return res;					
				
		}
        else if ( trg == UriType::URI_PLAYERS ) {
            json::object answ_obj;
            status = http::status::ok;
            switch (req.method()) {
                case http::verb::get:
                case http::verb::head:
                {
                    try {
                        auto header = req.base();
                        auto auth = header.at(http::field::authorization);
                        if (!auth.starts_with("Bearer") || auth.size() < (32 + 7)) {
                            status = http::status::unauthorized;
                            answ_obj["code"] = "invalidToken"s;
                            answ_obj["message"] = "Authorization header is missing"s;

                        } else {
                            auto token_str = std::string{auth.substr(auth.find_first_of(" ") + 1)};
                            Token token{token_str};
                            auto player = players_.FindByToken(token);
                            if (player != nullptr) {
                                auto playrs = players_.GetPlayers();
                                for (const auto &plyr: playrs) {
                                    json::object plyr_json;
                                    plyr_json["name"] = plyr.second.GetName();
                                    answ_obj[std::to_string(*(plyr.second.GetId()))] = plyr_json;
                                }

                            } else {
                                status = http::status::unauthorized;
                                answ_obj["code"] = "unknownToken"s;
                                answ_obj["message"] = "Player token has not been found"s;
                            }
                        }
                    } catch(...) {
                        status = http::status::unauthorized;
                        answ_obj["code"] = "invalidToken"s;
                        answ_obj["message"] = "Authorization header is missing"s;

//                        {"code": "unknownToken", "message": "Player token has not been found"}
                    }
                    break;
                }
                default: {
                    status = http::status::method_not_allowed;
                    answ_obj["code"] = "invalidMethod"s;
                    answ_obj["message"] = "Only GET, HEAD method is expected"s;
                    allow_method = "GET, HEAD"sv;
                    break;
                }
            }
            answer = req.method() == http::verb::head ? "" : json::serialize(answ_obj);
            body = answer;
            auto resp =  MakeStringResponse(status, body, req.version(), req.keep_alive(),
                                            rqs, content_type, allow_method);

            HandlerResult res = std::make_pair( content_type, std::move(resp) );
            return res;
        }
        else if ( trg == UriType::URI_STATE ) {
            json::object answ_obj;
            status = http::status::ok;
            switch (req.method()) {
                case http::verb::get:
                case http::verb::head:
                {
                    try {
                        auto header = req.base();
                        auto auth = header.at(http::field::authorization);
                        if (!auth.starts_with("Bearer") || auth.size() < (32 + 7)) {
                            status = http::status::unauthorized;
                            answ_obj["code"] = "invalidToken"s;
                            answ_obj["message"] = "Authorization header is missing"s;

                        } else {
                            auto token_str = std::string{auth.substr(auth.find_first_of(" ") + 1)};
                            Token token{token_str};
                            auto player = players_.FindByToken(token);
                            if (player != nullptr) {
                                json::object plyrs_jsn;
                                auto playrs = players_.GetPlayers();
                                for (const auto &plyr: playrs) {
                                    json::object plyr_json;
                                    auto dog = plyr.second.GetDog();
                                    json::array pos_arr;
                                    pos_arr.emplace_back(model::real_to_string<model::CoordReal>(dog->GetPos().x));
                                    pos_arr.emplace_back(model::real_to_string<model::CoordReal>(dog->GetPos().y));
                                    plyr_json["pos"] = pos_arr;
                                    json::array pos_speed;
                                    pos_speed.emplace_back(model::real_to_string<model::SpeedLine>(dog->GetSpeed().vx));
                                    pos_speed.emplace_back(model::real_to_string<model::SpeedLine>(dog->GetSpeed().vy));
                                    plyr_json["speed"] = pos_speed;
                                    plyr_json["dir"] = dog->GetDirStr();
                                    plyrs_jsn[std::to_string(*(plyr.second.GetId()))] = plyr_json;
                                }
                                answ_obj["players"]=plyrs_jsn;
                            } else {
                                status = http::status::unauthorized;
                                answ_obj["code"] = "unknownToken"s;
                                answ_obj["message"] = "Player token has not been found"s;
                            }
                        }
                    } catch(...) {
                        status = http::status::unauthorized;
                        answ_obj["code"] = "invalidToken"s;
                        answ_obj["message"] = "Authorization header is missing"s;

//                        {"code": "unknownToken", "message": "Player token has not been found"}
                    }
                    break;
                }
                default: {
                    status = http::status::method_not_allowed;
                    answ_obj["code"] = "invalidMethod"s;
                    answ_obj["message"] = "Only GET, HEAD method is expected"s;
                    allow_method = "GET, HEAD"sv;
                    break;
                }
            }
            answer = req.method() == http::verb::head ? "" : json::serialize(answ_obj);
            body = answer;
            auto resp =  MakeStringResponse(status, body, req.version(), req.keep_alive(),
                                            rqs, content_type, allow_method);

            HandlerResult res = std::make_pair( content_type, std::move(resp) );
            return res;
        }
        else
		switch (req.method()) 
		{
			case http::verb::get:
			{
				status = http::status::ok;

				if ( trg.starts_with(UriType::URI_JOIN) ) {
					
				}
				else if ( trg.starts_with(UriType::URI_MAPS) ) {
					if ( trg == UriType::URI_MAPS ) {
						MapsToStr(answer);
					}
					else  {
						auto map_id = trg.substr(UriType::URI_MAPS.size());
						map_id.remove_prefix(std::min(map_id.find_first_not_of("/"), map_id.size()));
						if ( MapToStr(map_id, answer) != 0 )
						{
							// карту не нашли
							status = http::status::not_found;
							json::object obj;
							obj["code"] = "mapNotFound"s;
							obj["message"] = "Map not found"s;
							answer = json::serialize(obj);
						}
					}
				} else if ( trg.starts_with(UriType::URI_START) ) {
						status = http::status::bad_request;
						json::object obj;
						obj["code"] = "badRequest"s;
						obj["message"] = "Bad request"s;
						answer = json::serialize(obj);					
				}
				body = answer;
				rqs = GET;
				break;
			}
			case http::verb::post:
			{
				
				break;
			}
			default:
			{
				status = http::status::method_not_allowed;
				body = "Invalid method"sv;
				rqs = OTHER;
				break;
			}
		}
		 
		 auto resp =  MakeStringResponse(status, body, req.version(), req.keep_alive(), rqs);
		 
		 HandlerResult res = std::make_pair( ContentType::TEXT_HTML, std::move(resp) );
		 return res;
	}
	
private:
    model::Game& game_;
	player::Players& players_;
};

} // namespace http_handler
