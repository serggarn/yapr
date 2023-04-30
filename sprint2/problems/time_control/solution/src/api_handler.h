#pragma once
#include "response.h"
#include "request.h"
#include "game.h"
#include "players.h"
#include <boost/json.hpp>
#include "http_types.h"
#include <sstream>
#include <optional>

namespace api_handler {
namespace beast = boost::beast;
namespace http = beast::http;
namespace sys = boost::system;
namespace net = boost::asio;

using namespace http_types;
namespace json = boost::json;

using HandlerResult = std::pair<std::string_view, http::response<http::string_body>>;


class ApiHandler {
private:
    std::optional<Token> TryExtractToken(const StringRequest& request);
    bool AllowToken(const Token& token);

    StringResponse MakeUnauthorizedError(const StringRequest& request);
    StringResponse MakeUnknownTokenError(const StringRequest& request);
    StringResponse MakeTickError(const StringRequest& request);
    StringResponse MakeUnallowedMethodError(const StringRequest& request, std::string_view allow_method);

    template <typename Fn>
    StringResponse ExecuteAuthorized(const StringRequest& request, Fn&& action) {
        if (auto token = TryExtractToken(request)) {
            if ( ! AllowToken(*token) ) {
                return MakeUnknownTokenError(request);
            }
            return action(*token);
        } else {
            return MakeUnauthorizedError(request);
        }
    }


    StringResponse GetPlayers(const StringRequest& request);

    StringResponse SetPlayerAction(const StringRequest& request);

    StringResponse GetGameState(const StringRequest& request);

    StringResponse SetTick(const StringRequest& request);

    void RoadsToJson(const model::Map& map, json::array& jsn_array);
	void BuildingsToJson(const model::Map& map, json::array& jsn_array);
	void OfficesToJson(const model::Map& map, json::array& jsn_array);	
	void MapsToStr(std::string& answ);
	int MapToStr(std::string_view map_id, std::string& answ);
public:
	ApiHandler (net::io_context& ioc, model::Game& game, player::Players& players) : ioc_{ioc}, game_{game}, players_{players} {}
	template <typename Body, typename Allocator>
    HandlerResult GetResponse(http::request<Body, http::basic_fields<Allocator>>&& req) {
        http::status status;
        std::string_view body;
        requests_e rqs{OTHER};
        std::string answer;
        std::string_view trg = req.target();
        std::string_view allow_method;
        auto content_type = ContentType::APPL_JSON;

        if (trg == UriType::URI_JOIN) {
            json::object answ_obj;
            switch (req.method()) {
                case http::verb::post: {
                    try {
                        auto jsn_values = json::parse(req.body());
                        auto userName = jsn_values.as_object()["userName"].as_string();
                        auto mapId = jsn_values.as_object()["mapId"].as_string();
                        std::cout << "req: " << mapId << "; " << userName << std::endl;
                        if (userName.size() == 0) {
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
                            std::shared_ptr<model::GameSession> gs{nullptr};
                            // если уже есть сессия, то найдём её
                            if (game_.FindMap(model::Map::Id{mapId.c_str()}) != nullptr) {
                                for (auto& session: game_.GetSessions()) {
                                    if (*(session->GetMap()->GetId()) == mapId) {
                                        std::cout << "find gs " << session->GetDogs().size() << std::endl;
                                        gs = session;
                                    }
                                }
                            }
                            if (gs == nullptr) {
                                std::cout << "gs == nullptr" <<std::endl;
                                model::GameSession gm_ses{model::GameSession::Id{mapId.c_str()}, map};
                                game_.AddSession(gm_ses);
                                gs = game_.FindGameSession(model::GameSession::Id{mapId.c_str()});
                            }
                            gs->AddDog(name);
                            auto dg = gs->FindDog(model::Dog::Id{name});
                            std::cout << "size of dogs: " << gs->GetDogs().size() << std::endl;
                            std::cout << "size of maps: " << game_.GetMaps().size() << std::endl;
                            std::cout << "size of sessions: " << game_.GetSessions().size() << std::endl;
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
            auto resp = MakeStringResponse(status, body, req.version(), req.keep_alive(),
                                           content_type, allow_method);

            HandlerResult res = std::make_pair(content_type, std::move(resp));
            return res;

        }
        else if (trg == UriType::URI_PLAYERS) {
            StringResponse resp;
            switch (req.method()) {
                case http::verb::get:
                case http::verb::head: {
                    try {
                        resp = GetPlayers(req);
                    } catch (...) {
                        resp = MakeUnauthorizedError(req);
                    }
                    break;
                }
                default: {
                    resp = MakeUnallowedMethodError(req, "GET, HEAD"sv);
                    break;
                }
            }
            HandlerResult res = std::make_pair(content_type, std::move(resp));
            return res;
        }
        else if (trg == UriType::URI_STATE) {
            StringResponse resp;
            switch (req.method()) {
                case http::verb::get:
                case http::verb::head: {
                    try {
                        resp = GetGameState(req);
                    } catch (...) {
                        resp = MakeUnauthorizedError(req);
                    }
                    break;
                }
                default: {
                    resp = MakeUnallowedMethodError(req, "GET, HEAD"sv);
                    break;
                }
            }
            HandlerResult res = std::make_pair(content_type, std::move(resp));
            return res;
        }
        else if (trg == UriType::URI_ACTION) {
            StringResponse resp;
            switch (req.method()) {
                case http::verb::post: {
                    try {
                        resp = SetPlayerAction(req);
                    } catch (...) {
                        resp = MakeUnauthorizedError(req);
                    }
                    break;
                }
                default: {
                    resp = MakeUnallowedMethodError(req, "POST"sv);
                    break;
                }
            }
            HandlerResult res = std::make_pair(content_type, std::move(resp));
            return res;
        } else if (trg == UriType::URI_TICK) {
            StringResponse resp;
            switch (req.method()) {
                case http::verb::post: {
                    try {
                        resp = SetTick(req);
                    } catch (...) {
                        resp = MakeTickError(req);
                    }
                    break;
                }
                default: {
                    resp = MakeUnallowedMethodError(req, "POST"sv);
                    break;
                }
            }
            HandlerResult res = std::make_pair(content_type, std::move(resp));
            return res;
        }else {
            StringResponse resp;
            switch (req.method()) {

                case http::verb::get: {
                    status = http::status::ok;

                    if (trg.starts_with(UriType::URI_MAPS)) {
                        if (trg == UriType::URI_MAPS) {
                            MapsToStr(answer);
                        } else {
                            auto map_id = trg.substr(UriType::URI_MAPS.size());
                            map_id.remove_prefix(std::min(map_id.find_first_not_of("/"), map_id.size()));
                            if (MapToStr(map_id, answer) != 0) {
                                // карту не нашли
                                status = http::status::not_found;
                                json::object obj;
                                obj["code"] = "mapNotFound"s;
                                obj["message"] = "Map not found"s;
                                answer = json::serialize(obj);
                            }
                        }
                    } else if (trg.starts_with(UriType::URI_START)) {
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
                default: {
                    resp = MakeUnallowedMethodError(req, "GET"sv);
                    break;
                }
            }

            resp = MakeStringResponse(status, body, req.version(), req.keep_alive());

            HandlerResult res = std::make_pair(ContentType::TEXT_HTML, std::move(resp));
            return res;
        }
	}
	
private:
    net::io_context& ioc_;
    model::Game& game_;
	player::Players& players_;
};

} // namespace http_handler
