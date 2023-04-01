#include "logger.h"

void InitLogger() {
	logging::add_common_attributes();
	// Настраиваем логи
	logging::add_console_log(
		std::clog,
		keywords::format = &MyFormatter
	); 
}

void MyFormatter(logging::record_view const& rec, logging::formatting_ostream& strm) {
    // Выводить LineID стало проще.
//     strm << rec[line_id] << ": ";

    // Момент времени приходится вручную конвертировать в строку.
    // Для получения истинного значения атрибута нужно добавить
    // разыменование. 
    auto ts = *rec[timestamp];

	json::object record;
	record["timestamp"] = to_iso_extended_string(ts);
	record["data"] = *rec[additional_data];
	record["message"] = *rec[expr::smessage];
//     strm << to_iso_extended_string(ts) << ": ";
	strm << json::serialize(record);
    // Выводим уровень, заключая его в угловые скобки.
//     strm << "<" << rec[logging::trivial::severity] << "> ";

    // Выводим само сообщение.
//     strm << rec[expr::smessage];
} 
