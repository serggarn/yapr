#include <iostream>

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/date_time.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/support/date_time.hpp>
#include <thread>
#include <string_view>



#include <boost/locale/generator.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/log/common.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/support/date_time.hpp>
namespace logging = boost::log;
using namespace std::literals;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;

BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
// BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)

BOOST_LOG_ATTRIBUTE_KEYWORD(file, "File", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(line, "Line", int) 


		
void InitBoostLogFilter() {
	logging::add_common_attributes();
    logging::core::get()->set_filter(
        logging::trivial::severity >= logging::trivial::info
    );
} 

void MyFormatter(logging::record_view const& rec, logging::formatting_ostream& strm) {
    // Выводить LineID стало проще.
    strm << rec[line_id] << ": ";

    // Момент времени приходится вручную конвертировать в строку.
    // Для получения истинного значения атрибута нужно добавить
    // разыменование. 

// 	std::cout << "ok?"<<*rec[timestamp]<<std::endl;
    auto ts = *rec[timestamp];
// 	std::cout << "null?"<< ts << std::endl;
    strm << to_iso_extended_string(ts) << ": ";
// 	strm << to_iso_extended_string(rec[timestamp]);
    // Выводим уровень, заключая его в угловые скобки.
    strm << "<" << rec[logging::trivial::severity] << "> ";

    // Выводим само сообщение.
    strm << rec[expr::smessage];
} 



int main() {

	logging::add_file_log(
		keywords::file_name = "sample_%N.log",
		keywords::format = "[%TimeStamp%]: %Message%",
		keywords::open_mode = std::ios_base::app | std::ios_base::out,
		// ротируем по достижению размера 10 мегабайт
		keywords::rotation_size = 10 * 1024 * 1024,
		// ротируем ежедневно в полдень
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(12, 0, 0)
	);
// 	logging::add_console_log(
// 		std::clog,
// // 		keywords::file_name = "sample.log",
// // 		keywords::format = &MyFormatter
// 		keywords::format = expr::stream 
// 			            << expr::format_date_time(timestamp, "%Y-%m-%d, %H:%M:%S.%f")
//             << " <" << logging::trivial::severity.or_default(0)
//             << "> " << expr::message
// 	); 
	BOOST_LOG_TRIVIAL(info) 
        << logging::add_value(file, __FILE__) 
        << logging::add_value(line, __LINE__) 
        << "Something happend"sv; 
    std::cout << std::hex << std::this_thread::get_id() << std::endl;
	InitBoostLogFilter();
    BOOST_LOG_TRIVIAL(trace) << "Сообщение уровня trace"sv;
    BOOST_LOG_TRIVIAL(debug) << "Сообщение уровня debug"sv;
    BOOST_LOG_TRIVIAL(info) << "Сообщение уровня info"sv;
    BOOST_LOG_TRIVIAL(warning) << "Сообщение уровня warning"sv;
    BOOST_LOG_TRIVIAL(error) << "Сообщение уровня error"sv;
    BOOST_LOG_TRIVIAL(fatal) << "Сообщение уровня fatal"sv;
	logging::add_console_log(
		std::clog,
// 		keywords::file_name = "sample.log",
// 		keywords::format = &MyFormatter
		keywords::format ="[%TimeStamp%]: %Message%"
	);

    BOOST_LOG_TRIVIAL(trace) << "Сообщение уровня trace"sv;
    BOOST_LOG_TRIVIAL(debug) << "Сообщение уровня debug"sv;
    BOOST_LOG_TRIVIAL(info) << "Сообщение уровня info"sv;
    BOOST_LOG_TRIVIAL(warning) << "Сообщение уровня warning"sv;
    BOOST_LOG_TRIVIAL(error) << "Сообщение уровня error"sv;
    BOOST_LOG_TRIVIAL(fatal) << "Сообщение уровня fatal"sv;	
} 

