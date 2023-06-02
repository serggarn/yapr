#pragma once
#include "../response.h"
#include "../request.h"
#include "../game_play/game.h"
#include "../model/players.h"
#include <boost/json.hpp>
#include "../http_types.h"
#include <sstream>
#include <optional>
#include "../json/json_tags.h"

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
    bool AllowToken(const Token& token);

    static std::optional<Token> TryExtractToken(const StringRequest& request);
    static StringResponse MakeError(const StringRequest& request, const http::status,
                                    const std::string& code, const std::string& message);
    inline static StringResponse MakeUnauthorizedError(const StringRequest& request)
    { return MakeError(request, http::status::bad_request, "invalidArgument"s, "Failed to parse tick request JSON"s); }
    static StringResponse MakeUnknownTokenError(const StringRequest& request)
    { return MakeError(request, http::status::unauthorized, "unknownToken"s, "Player token has not been found"s); }
    static StringResponse MakeTickError(const StringRequest& request)
    { return MakeError(request, http::status::bad_request, "invalidArgument"s, "Failed to parse tick request JSON"s); }
    static StringResponse MakeNameError(const StringRequest& request)
    { return MakeError(request, http::status::bad_request, "invalidArgument"s, "Invalid name"s); }
    static StringResponse MakeJoinError(const StringRequest& request)
    { return MakeError(request, http::status::bad_request, "invalidArgument"s, "Join game request parse error"s); }
    static StringResponse MakeUnallowedMethodError(const StringRequest& request, std::string_view allow_method);
    static StringResponse MakeInvalidEndpointError(const StringRequest& request)
    { return MakeError(request, http::status::bad_request, "badRequest"s, "Invalid endpoint"s); }
    static StringResponse MakeBadRequestError(const StringRequest& request)
    { return MakeError(request, http::status::bad_request, "badRequest"s, "Bad request"s); }
    static StringResponse MakeMapNotFoundError(const StringRequest& request)
    { return MakeError(request, http::status::not_found, "mapNotFound"s, "Map not found"s); }

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

    StringResponse JoinGameUseCase(const StringRequest& request);
    StringResponse PlayersGameUseCase(const StringRequest& request);
    StringResponse ActionGameUseCase(const StringRequest& request);
    StringResponse StateGameUseCase(const StringRequest& request);
    StringResponse TickGameUseCase(const StringRequest& request);
    StringResponse MapsGameUseCase(const StringRequest& request);
    StringResponse RecordsGameUseCase(const StringRequest& request);

    bool CheckTestEnv();

    void RoadsToJson(const model::Map& map, json::array& jsn_array);
	void BuildingsToJson(const model::Map& map, json::array& jsn_array);
	void OfficesToJson(const model::Map& map, json::array& jsn_array);
    void LootDefinitionsToJson(const model::Map& map, json::array& jsn_array);
    void LootsToJson(const model::Map& map, json::object& jsn_array);
	void MapsToStr(std::string& answ);
    json::object PlayerStateToJson(const Player& player);
	std::optional<std::string> MapToStr(std::string_view map_id);
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
        StringResponse response; 

        if (trg == UriType::URI_JOIN) {
            response  = JoinGameUseCase(req);
        }
        else if (trg == UriType::URI_PLAYERS) {
            response  = PlayersGameUseCase(req);
        }
        else if (trg == UriType::URI_STATE) {
            response  = StateGameUseCase(req);
        }
        else if (trg == UriType::URI_ACTION) {
            response  = ActionGameUseCase(req);
        }
        else if (trg == UriType::URI_TICK) {
            response  = TickGameUseCase(req);
        }
        else if (trg == UriType::URI_RECORDS) {
            response  = RecordsGameUseCase(req);
        }
        else if (trg.starts_with(UriType::URI_MAPS)) {
            response = MapsGameUseCase(req);
        } else if (trg.starts_with(UriType::URI_START)) {
            response = MakeBadRequestError(req);
        }
        
        HandlerResult res = std::make_pair(ContentType::TEXT_HTML, std::move(response));
        return res;
	}
	
private:
    net::io_context& ioc_;
    model::Game& game_;
	player::Players& players_;
};

} // namespace http_handler
