//
// Created by serg on 01.05.23.
//

#ifndef GAME_SERVER_TICKER_H
#define GAME_SERVER_TICKER_H

#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>
#include <chrono>
//#include <iostream>



namespace ticker {
    namespace net = boost::asio;
    namespace sys = boost::system;
    using namespace std::chrono;

    class Ticker : public std::enable_shared_from_this<Ticker> {
    public:
        using Strand = net::strand<net::io_context::executor_type>;
        using Handler = std::function<void(std::chrono::milliseconds delta)>;

        Ticker(Strand& strand, std::chrono::milliseconds period, Handler handler) :
                strand_{strand}, period_{period}, handler_{handler} {};

        void Start() {
//            std::cout << "ticker start " <<std::endl;
            last_tick_ = duration_cast<milliseconds>(steady_clock::now().time_since_epoch());
            /* Выполнить SchedulTick внутри strand_ */
//            net::bind_executor(strand_, ScheduleTick());
            net::dispatch(strand_, [this]() {
                ScheduleTick();
            });

        }

    private:
        void ScheduleTick() {
//            std::cout << "scheduletick" <<std::endl;
            /* выполнить OnTick через промежуток времени period_ */
            timer_.expires_after(period_);
//            std::cout << "scheduletick" <<std::endl;
            timer_.async_wait([self = shared_from_this()](sys::error_code ec) {
//                std::cout << "before on tick()"<<std::endl;
                self->OnTick(ec);
            });
//            std::cout << "scheduletick 4" <<std::endl;
        }

        void OnTick(sys::error_code ec) {
            auto current_tick = duration_cast<milliseconds>(steady_clock::now().time_since_epoch());
//            std::cout << "before handler"<<std::endl;
            handler_(current_tick - last_tick_);
//            std::cout << "after handler"<<std::endl;
            last_tick_ = current_tick;
            ScheduleTick();
        }

        Strand& strand_;
        net::steady_timer timer_{strand_};
        std::chrono::milliseconds period_;
        Handler handler_;
        std::chrono::milliseconds last_tick_;
    };
}

#endif //GAME_SERVER_TICKER_H
