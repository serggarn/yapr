#pragma once
#include "http_server.h"
#include "model.h"
#include <boost/json.hpp>

namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
using StringResponse = http::response<http::string_body>; 

using namespace std::literals; 
namespace json = boost::json;

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game)
        : game_{game} {
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;
	
	// Структура ContentType задаёт область видимости для констант,
	// задающий значения HTTP-заголовка Content-Type
	struct ContentType {
		ContentType() = delete;
		constexpr static std::string_view TEXT_HTML = "text/html"sv;
		constexpr static std::string_view APPL_JSON = "application/json"sv;
		// При необходимости внутрь ContentType можно добавить и другие типы контента
	};
	
	typedef enum : uint8_t {
		GET,
		HEAD,
		OTHER,
	} requests_e;
	
	struct UriType {
		UriType() = delete;
		constexpr static std::string_view URI_START = "/api/";
		constexpr static std::string_view URI_MAPS = "/api/v1/maps";
	};
	
	// Создаёт StringResponse с заданными параметрами
	StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
									bool keep_alive,
									requests_e rqs_tp = OTHER,
									std::string_view content_type = ContentType::APPL_JSON) {
		StringResponse response(status, http_version);
		response.set(http::field::content_type, content_type);
		if ( rqs_tp == OTHER )
		{
			response.set(http::field::allow, "GET, HEAD"sv);
		}
		if ( rqs_tp != HEAD )
		{
			response.body() = body;
		}
// 		std::cout << "Body: " << body << std::endl;
		response.content_length(body.size());
		response.keep_alive(keep_alive);
		return response;
	}
	
    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {

        // Обработать запрос request и отправить ответ, используя send
		std::pair<http::status, std::string_view> res{};
		requests_e rqs {OTHER};
		std::string answer;
		switch (req.method()) 
		{
			case http::verb::get:
			{
				res.first = http::status::ok;
				std::string_view trg = req.target();

				if ( trg.starts_with(UriType::URI_MAPS) ) {
					if ( trg == UriType::URI_MAPS ) {
						MapsToStr(answer);
					}
					else  {
						auto map_id = trg.substr(UriType::URI_MAPS.size());
						map_id.remove_prefix(std::min(map_id.find_first_not_of("/"), map_id.size()));
						if ( MapToStr(map_id, answer) != 0 )
						{
							// карту не нашли
							res.first = http::status::not_found;
							json::object obj;
							obj["code"] = "mapNotFound";
							obj["message"] = "Map not found";
							answer = json::serialize(obj);
						}
					}
				} else if ( trg.starts_with(UriType::URI_START) ) {
						res.first = http::status::bad_request;
						json::object obj;
						obj["code"] = "badRequest";
						obj["message"] = "Bad request";
						answer = json::serialize(obj);					
				}
				res.second = answer;
				rqs = GET;
				break;
			}
			default:
			{
				res.first = http::status::method_not_allowed;
				res.second = "Invalid method";
				rqs = OTHER;
				break;
			}
		}
		send(MakeStringResponse(res.first, res.second, req.version(), req.keep_alive(), rqs));
    }

private:
    model::Game& game_;
	
	void MapsToStr(std::string& answ);
	int MapToStr(const std::string_view map_id, std::string& answ);
};

}  // namespace http_handler
