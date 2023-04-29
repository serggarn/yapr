#pragma once
#include "file_handler.h"
#include "model.h"
#include "logger.h"
// #include "response.h"
#include <sstream>
#include "api_handler.h"
#include "http_server.h"
#include "response.h"

#include <chrono>
#include <variant>
// using namespace api_handler;

namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
namespace sys = boost::system;
namespace net = boost::asio;


using namespace std::literals; 
namespace json = boost::json;
using namespace http_types;

template<class SomeRequestHandler/*, typename Request, typename Response*/>
class LoggingRequestHandler {
	template <typename Body, typename Allocator/*, typename Send*/>
	static void LogRequest(http::request<Body, http::basic_fields<Allocator>> r, std::string& address) {
		json::object data;
		data["ip"] = address;
		auto uri = r.target();
		data["URI"] = std::string(uri);
		auto method = http::to_string(r.method());
		data["method"] = std::string(method);
		BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, data)
								<< "request received"sv;				
	}
	
// 	template <typename Body>
	static void LogResponse(std::pair<int, std::string_view>& r, time_t time) {
		json::object data;
		data["response_time"] = time;
		data["code"] = r.first;
		data["content_type"] = std::string{r.second};
		BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, data)
								<< "response sent"sv;	
	}
// 	template <typename Type>
// 	static void LogResponse(const Response<Type>& resp);
public:
	LoggingRequestHandler(SomeRequestHandler& obj) : decorated_(obj) {}
	template <typename Body, typename Allocator, typename Send>
     void operator () (http::request<Body, http::basic_fields<Allocator>>&& req, std::string address, Send&& send) /*(Request req)*/ {
         LogRequest(req, address);
		 auto start_ts = std::chrono::system_clock::now();
         auto resp = decorated_(std::move(req), std::move(send));
		 auto res_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_ts).count();
         LogResponse(resp, (time_t)res_time);
// 		 send(std::move(resp));
//          return resp;
     }

private:
     SomeRequestHandler& decorated_;
};

class RequestHandler {
public:
    explicit RequestHandler(net::io_context& ioc, model::Game& game, file_handler::Files& files, player::Players& players)
        : ioc_{ioc}, game_{game}, files_{files}, players_{players} {
    }

//     RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;
	
	
	template <typename Body, typename Allocator, typename Send>
    std::pair<int, std::string_view> operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {

        // Обработать запрос request и отправить ответ, используя send
		std::pair<http::status, std::string_view> res{};
		std::pair<http::status, std::string_view> ret_value;
		requests_e rqs {OTHER};
		std::string answer;
		std::string_view trg = req.target();
// 		std::cout << "trg: " << trg <<std::endl;
		// все запрос к API обрабатываем отдельно
		if ( trg.starts_with(UriType::URI_START) ) {
			api_handler::ApiHandler handler {ioc_, game_, players_};
			auto resp = handler.GetResponse(std::move(req));
			send(std::move(resp.second));
// 								std::cout <<"2"<<std::endl;
			return std::make_pair(resp.second.result_int(), resp.first);
		}
			
		switch (req.method()) 
		{
			case http::verb::get:
			{
				res.first = http::status::ok;

				// ищем файл
				auto stat_path = files_.GetPath(trg);
				if ( stat_path.first == http::status::ok ) {
					res.second = stat_path.second;
// 						std::cout << "res: " << res.second <<std::endl;
					auto content = files_.GetContentType(res.second);
					auto resp = MakeFileResponse(res.first, res.second, req.version(), content);
					send(std::move(resp));
// 								std::cout <<"2"<<std::endl;
					return std::make_pair(resp.result_int(), content);
				}
				res.first = stat_path.first;
				res.second = "File not found"sv;
// 						std::cout << "error" <<std::endl;
// 						std::stringstream error_msg;
// 					std::cout << "File " << trg << " not found!";
// 						throw std::runtime_error(error_msg.str());
				auto resp =  MakeStringResponse(res.first, res.second, req.version(), req.keep_alive(), ContentType::TEXT_PLAIN);
				send(std::move(resp));
				return std::make_pair(resp.result_int(), ContentType::TEXT_PLAIN);
				res.second = answer;
				rqs = GET;
				break;
			}
			default:
			{
				res.first = http::status::method_not_allowed;
				res.second = "Invalid method"sv;
				rqs = OTHER;
				break;
			}
		}
		auto resp =  MakeStringResponse(res.first, res.second, req.version(), req.keep_alive());
		send(resp);
		return std::make_pair(resp.result_int(), ContentType::APPL_JSON/*resp.get(http::field::content_type)*/);
		
    }
private:
    net::io_context& ioc_;
    model::Game& game_;
	file_handler::Files& files_;
	player::Players& players_;
	
};

}  // namespace http_handler
