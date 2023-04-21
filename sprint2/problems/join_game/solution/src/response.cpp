#include "response.h"

// namespace http_handler {
StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
								bool keep_alive,
								requests_e rqs_tp,
								std::string_view content_type, 
								std::string_view allow_method) {
	StringResponse response(status, http_version);
	response.set(http::field::content_type, content_type);
	response.set(http::field::cache_control, "no-cache"sv);
	if ( status == http::status::method_not_allowed)
		response.set(http::field::allow, allow_method);
	if ( rqs_tp == OTHER )
	{
		response.set(http::field::allow, "GET, HEAD"sv);
	}
//	if ( rqs_tp != HEAD )
//	{
		response.body() = body;
//	}
 		std::cout << "Body: " << body << std::endl;
	response.content_length(body.size());
	response.keep_alive(keep_alive);
	return response;
}

FileResponse MakeFileResponse(http::status status, std::string_view file_name, unsigned http_version, std::string_view content_type) {

	FileResponse response(status, http_version);
	response.insert(http::field::content_type, content_type);
// 		std::cout <<"1"<<std::endl;

	http::file_body::value_type file;
// 		std::cout <<"2"<<std::endl;

	std::string f_name = std::string{file_name.begin(),file_name.end()};
// 		std::cout <<"2"<<std::endl;
	if (sys::error_code ec; file.open(f_name.c_str(), beast::file_mode::read, ec), ec) {
		std::cout << "Failed to open file "sv << file_name << std::endl;
		throw std::runtime_error("File " + f_name + " do not open!");
	}
// 		std::cout <<"3"<<std::endl;
	response.body() = std::move(file);
// 		std::cout <<"4"<<std::endl;

	// Метод prepare_payload заполняет заголовки Content-Length и Transfer-Encoding
	// в зависимости от свойств тела сообщения
	response.prepare_payload();
// 		std::cout <<"5"<<std::endl;

	return response;
}
// } // namespace http_server
