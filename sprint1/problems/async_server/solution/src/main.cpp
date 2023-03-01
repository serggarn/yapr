#include "sdk.h"
//
#include <boost/asio/signal_set.hpp>
#include <iostream>
// #include <mutex>
#include <thread>
#include <vector>

#include "http_server.h"

namespace net = boost::asio;
namespace sys = boost::system;
using namespace std::literals;

namespace http_server{
// namespace net = boost::asio;
// namespace sys = boost::system;
// using tcp = net::ip::tcp;
// namespace beast = boost::beast;
// namespace http = beast::http;

// using namespace std::literals;

// Запрос, тело которого представлено в виде строки
using StringRequest = http::request<http::string_body>;
// Ответ, тело которого представлено в виде строки
using StringResponse = http::response<http::string_body>;


struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    // При необходимости внутрь ContentType можно добавить и другие типы контента
};


typedef enum : uint8_t {
	GET,
	HEAD,
	OTHER,
} requests_e;

// Создаёт StringResponse с заданными параметрами
StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                  bool keep_alive,
								  requests_e rqs_tp = OTHER,
                                  std::string_view content_type = ContentType::TEXT_HTML) {
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

    response.content_length(body.size());
    response.keep_alive(keep_alive);
    return response;
}

StringResponse HandleRequest(StringRequest&& req) {
    const auto text_response = [&req](http::status status, std::string_view text, requests_e rqs_tp) {
        return MakeStringResponse(status, text, req.version(), req.keep_alive(), rqs_tp);
    };

	std::pair<http::status, std::string_view> res{};
	requests_e rqs {OTHER};
	switch (req.method()) 
	{
		case http::verb::get:
		{
			res.first = http::status::ok;
			std::string_view trg = req.target();
			trg.remove_prefix(std::min(trg.find_first_not_of("/"),trg.size()));
			std::string answer = "Hello, " + std::string{trg};
			res.second = answer;
			rqs = GET;
// 						 ""sv
// 						 req.target();
			break;
		}
		case http::verb::head:
		{
			res.first = http::status::ok;
			std::string_view trg = req.target();
			trg.remove_prefix(std::min(trg.find_first_not_of("/"),trg.size()));
			std::string answer = "Hello, " + std::string{trg};
			res.second = answer;
			rqs = HEAD;
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
    // Здесь можно обработать запрос и сформировать ответ, но пока всегда отвечаем: Hello
    return text_response(res.first, res.second, rqs);
} 


// Запускает функцию fn на n потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);
    std::vector<std::jthread> workers;
    workers.reserve(n - 1);
    // Запускаем n-1 рабочих потоков, выполняющих функцию fn
    while (--n) {
        workers.emplace_back(fn);
    }
    fn();
}

}  // namespace

int main() {
    const unsigned num_threads = std::thread::hardware_concurrency();

    net::io_context ioc(num_threads);

    // Подписываемся на сигналы и при их получении завершаем работу сервера
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
        if (!ec) {
            ioc.stop();
        }
    });

    const auto address = net::ip::make_address("0.0.0.0");
    constexpr net::ip::port_type port = 8080;
    http_server::ServeHttp(ioc, {address, port}, [](auto&& req, auto&& sender) {
        // sender(HandleRequest(std::forward<decltype(req)>(req)));
    });

    // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
    std::cout << "Server has started..."sv << std::endl;

    http_server::RunWorkers(num_threads, [&ioc] {
        ioc.run();
    });
}
