#pragma once

#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <optional>
#include <mutex>
#include <thread>
#include <iostream>
using namespace std::literals;

#define LOG(...) Logger::GetInstance().Log(__VA_ARGS__)

class Logger {
    auto GetTime() const {
// 		std::lock_guard<std::mutex> lk (mut_ts);
        if (manual_ts_) {
            return *manual_ts_;
        }

        return std::chrono::system_clock::now();
    }

    auto GetTimeStamp() const {
        const auto now = GetTime();
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        return std::put_time(std::gmtime(&t_c), "%F %T");
    }

    // Для имени файла возьмите дату с форматом "%Y_%m_%d"
    std::string GetFileTimeStamp() const;

    Logger() = default;
    Logger(const Logger&) = delete;

public:
    static Logger& GetInstance() {
        static Logger obj;
        return obj;
    }

    // Установите manual_ts_. Учтите, что эта операция может выполняться
    // параллельно с выводом в поток, вам нужно предусмотреть 
    // синхронизацию.
    void SetTimestamp(std::chrono::system_clock::time_point ts);

    // Выведите в поток все аргументы.
    template<class... Ts>
    void Log(const Ts&... args) {
		std::lock_guard lk(mut_write);
		auto t1 = GetTimeStamp();
		std::stringstream ss1, ss2;
		ss1 <<t1;
// 		std::lock_guard lk1(mut_ts);
		std::stringstream sfile;
		sfile << base_path << file_name << GetFileTimeStamp() << extension;
// 		std::cout << "filename: " << sfile.str() <<std::endl;
		std::ofstream log_file_{sfile.str(), std::ios::app};
// 		std::lock_guard lk1 (mut_ts);
		auto t2 = GetTimeStamp();
		ss2 <<t2;
		if (ss1.str().substr(10) != ss2.str().substr(10)) {
			(( std::cout << args ), ...);
			std::cout << std::endl;
		}
		log_file_ << GetTimeStamp() << ": ";
		(( log_file_ << args ), ...);
		log_file_ << std::endl;
	}

private:
    std::optional<std::chrono::system_clock::time_point> manual_ts_;
	std::mutex mut_ts, mut_write;
	static constexpr std::string_view base_path = "./"sv;
	static constexpr std::string_view file_name = "sample_log_"sv;
	static constexpr std::string_view extension = ".log"sv;
};
