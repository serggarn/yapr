#include "http_server.h"


namespace http_server {

void SessionBase::Read() {
	// Очищаем запрос от прежнего значения (метод Read может быть вызван несколько раз)
	request_ = {};
	stream_.expires_after(30s);
	// Считываем request_ из stream_, используя buffer_ для хранения считанных данных
	http::async_read(stream_, buffer_, request_,
						// По окончании операции будет вызван метод OnRead
						beast::bind_front_handler(&SessionBase::OnRead, GetSharedThis()));
}

void SessionBase::OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read) {
	if (ec == http::error::end_of_stream) {
		// Нормальная ситуация - клиент закрыл соединение
		return Close();
	}
	if (ec) {
		LogError(ec, "read"sv);
// 		return ReportError(ec, "read"sv);
	}
	HandleRequest(std::move(request_));
}

void SessionBase::OnWrite(bool close, beast::error_code ec, [[maybe_unused]] std::size_t bytes_written) {
	if (ec) {
		LogError(ec, "write"sv);
// 		return ReportError(ec, "write"sv);
	}

	if (close) {
		// Семантика ответа требует закрыть соединение
		return Close();
	}

	// Считываем следующий запрос
	Read();
}

void SessionBase::Close() {
	beast::error_code ec;
	stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
}

// Разместите здесь реализацию http-сервера, взяв её из задания по разработке асинхронного сервера
void SessionBase::Run() {
    // Вызываем метод Read, используя executor объекта stream_.
    // Таким образом вся работа со stream_ будет выполняться, используя его executor
    net::dispatch(stream_.get_executor(),
                  beast::bind_front_handler(&SessionBase::Read, GetSharedThis()));
}

tcp::endpoint SessionBase::GetRemoteIp() {
	return stream_.socket().remote_endpoint();
}

void LogError(beast::error_code ec, std::string_view where) {
	json::object data;
	data["code "] = ec.value();
	data["text "] = ec.message();
	data["where"] = std::string{where};
	BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, data)
							<< "error"sv;	
}

}  // namespace http_server
