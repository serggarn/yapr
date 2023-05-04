#include "sdk.h"
//
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <thread>

#include "json_loader.h"
// #include "api_handler.h"
#include "request_handler.h"
#include "file_handler.h"
#include "logger.h"
#include "system/cmd_line.h"
#include "system/settings.h"
#include "app_layer/ticker.h"


using namespace std::literals;
namespace net = boost::asio;
namespace sys = boost::system;
namespace fs =std::filesystem; 

namespace {

// Запускает функцию fn на numbers потоках, включая текущий
template <typename Fn>
void RunWorkers(unsigned numbers, const Fn& fn) {
    numbers = std::max(1u, numbers);
    std::vector<std::jthread> workers;
    workers.reserve(numbers - 1);
    // Запускаем numbers-1 рабочих потоков, выполняющих функцию fn
    while (--numbers) {
        workers.emplace_back(fn);
    }
    fn();
}

}  // namespace

int main(int argc, const char* argv[]) {
// 	fs::path base_path{"/this/is/some/path/"s};
//     fs::path rel_path{"../../another/path"s};
// 	std::cout << base_path / rel_path <<std::endl;
//     fs::path abs_path = fs::weakly_canonical(base_path / rel_path);
//     std::cout << abs_path << std::endl; // Выведет "/this/is/another/path"
    InitLogger();
    try {
        if (auto args = sys_utils::ParseCommandLine(argc, argv)) {

            // 1. Загружаем карту из файла и построить модель игры
            if ( ! exists(args->config) || ! is_regular_file(args->config) )
                throw std::runtime_error { "Config file not exist"s };
            model::Game game = json_loader::LoadGame(args->config);
            player::Players players{};

//            if ( ! exists(args->www_root) || ! is_directory(args->www_root) )
//                throw std::runtime_error { "www-root directory not exist"s };
            file_handler::Files files{args->www_root};

            // 2. Инициализируем io_context
            const unsigned num_threads = std::thread::hardware_concurrency();
            net::io_context ioc(num_threads);

            // 3. Добавляем асинхронный обработчик сигналов SIGINT и SIGTERM
            net::signal_set signals(ioc, SIGINT, SIGTERM);
            signals.async_wait([&ioc](const sys::error_code &ec, [[maybe_unused]] int signal_number) {
                if (!ec) {
                    json::value custom_data{{"code"s, 0}};
                    BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, custom_data)
                                            << "server exited"sv;
                    ioc.stop();
                }
            });

            // 4. Создаём обработчик HTTP-запросов и связываем его с моделью игры
            http_handler::RequestHandler handler{ioc, game, files, players};
// 		http_handler::RequestHandler handler{game, files};
            http_handler::LoggingRequestHandler<http_handler::RequestHandler> logging_handler{handler};

            // Запустим timer

            auto sett = settings::Settings::GetInstance();
            if ( ! sett->IsTestEnv() ) {
//                auto handle_tick
//                        = [strand = net::make_strand(ioc), &game](std::chrono::milliseconds _ms) {
//                            net::dispatch(strand, [&game, &_ms] {
//                                game.Tick(_ms);
//                            });
//                        };
                auto handle_tick
                        = [&game](std::chrono::milliseconds _ms) {
                            game.Tick(_ms);
                        };
                std::make_shared<ticker::Ticker> (net::make_strand(ioc), std::chrono::milliseconds(args->tick), handle_tick)->Start();
//                tick.Start();
            }
            // 5. Запустить обработчик HTTP-запросов, делегируя их обработчику запросов
            const auto address = net::ip::make_address("0.0.0.0");
            constexpr net::ip::port_type port = 8080;
            http_server::ServerHttp(ioc, {address, port}, [&logging_handler](auto &&req, auto &&addr, auto &&send) {
                logging_handler(std::forward<decltype(req)>(req), std::forward<decltype(addr)>(addr),
                                std::forward<decltype(send)>(send));
// 			   handler(std::forward<decltype(req)>(req))));
            });


            json::object data;
            data["address"] = address.to_string();
            data["port"] = port;
            BOOST_LOG_TRIVIAL(info)
                << logging::add_value(additional_data, data)
                << "server started"sv;
            // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
//         std::cout << "Server has started..."sv << std::endl;

            // 6. Запускаем обработку асинхронных операций
            RunWorkers(std::max(1u, num_threads), [&ioc] {
                ioc.run();
            });
        }
        else {
            throw std::runtime_error{"Failed to parse command args. Usage: "s + argv[0] + " --help"};
        }
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
