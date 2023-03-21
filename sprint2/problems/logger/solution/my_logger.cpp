#include "my_logger.h"

#include <sstream>
#include <iomanip>

namespace cr = std::chrono;
std::string Logger::GetFileTimeStamp() const {
	std::stringstream ss;
	auto now = GetTime();
	const auto t_c = std::chrono::system_clock::to_time_t(now);
    ss << std::put_time(std::localtime(&t_c), "%Y_%m_%d");
	return ss.str();
}

void Logger::SetTimestamp(std::chrono::system_clock::time_point ts) {
	std::lock_guard lk(mut_ts);
	manual_ts_ = ts;
}
