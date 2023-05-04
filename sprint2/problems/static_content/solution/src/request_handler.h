#pragma once
#include "http_server.h"
#include "model.h"
#include "file_handler.h"
#include <boost/json.hpp>
#include <sstream>
#include "http_types.h"

namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
namespace sys = boost::system;


using StringResponse = http::response<http::string_body>; 
using FileResponse = http::response<http::file_body>;

using namespace std::literals; 
namespace json = boost::json;
using namespace http_types;

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game, file_handler::Files& files)
        : game_{game}, files_{files} {
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;
	
	
	http_handler::StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
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

	FileResponse MakeFileResponse(http::status status, std::string_view file_name, unsigned http_version, std::string_view content_type) {
	
		FileResponse response(status, http_version);
		response.insert(http::field::content_type, content_type);
		std::cout <<"1"<<std::endl;

		http::file_body::value_type file;
		std::cout <<"2"<<std::endl;

		std::string f_name = std::string{file_name.begin(),file_name.end()};
		std::cout <<"2"<<std::endl;
		if (sys::error_code ec; file.open(f_name.c_str(), beast::file_mode::read, ec), ec) {
			std::cout << "Failed to open file "sv << file_name << std::endl;
			throw std::runtime_error("File " + f_name + " do not open!");
		}
		std::cout <<"3"<<std::endl;
		response.body() = std::move(file);
		std::cout <<"4"<<std::endl;

		// Метод prepare_payload заполняет заголовки Content-Length и Transfer-Encoding
		// в зависимости от свойств тела сообщения
		response.prepare_payload();
		std::cout <<"5"<<std::endl;

		return response;
	}
	
    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {

        // Обработать запрос request и отправить ответ, используя send
		std::pair<http::status, std::string_view> res{};
		requests_e rqs {OTHER};
		std::string answer;
		std::string_view trg = req.target();
		std::cout << "trg: " << trg <<std::endl;
		switch (req.method()) 
		{
			case http::verb::get:
			{
				res.first = http::status::ok;

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
				} else {
					// ищем файл
					auto stat_path = files_.GetPath(trg);
					if ( stat_path.first == http::status::ok ) {
						res.second = stat_path.second;
						std::cout << "res: " << res.second <<std::endl;
						send(MakeFileResponse(res.first, res.second, req.version(), files_.GetContentType(res.second)));
								std::cout <<"2"<<std::endl;
						return;
					}
					res.first = stat_path.first;
					res.second = "File not found"sv;
// 						std::cout << "error" <<std::endl;
// 						std::stringstream error_msg;
					std::cout << "File " << trg << " not found!";
// 						throw std::runtime_error(error_msg.str());
					send(MakeStringResponse(res.first, res.second, req.version(), req.keep_alive(), rqs, ContentType::TEXT_PLAIN));
					return;
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
	file_handler::Files& files_;
	
	void RoadsToJson(const model::Map& map, json::array& jsn_array);
	void BuildingsToJson(const model::Map& map, json::array& jsn_array);
	void OfficesToJson(const model::Map& map, json::array& jsn_array);	
	void MapsToStr(std::string& answ);
	int MapToStr(std::string_view map_id, std::string& answ);

};

}  // namespace http_handler
