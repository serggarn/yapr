#pragma once
// #include "http_server.h"
// #include <boost/asio/ip/tcp.hpp>
// #include <boost/asio/strand.hpp>
// #include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include "http_types.h"

// namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
namespace sys = boost::system;

using StringResponse = http::response<http::string_body>; 
using FileResponse = http::response<http::file_body>;
using EmptyResponse = http::response<http::empty_body>;



using namespace http_types;

StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
								bool keep_alive,
//								requests_e rqs_tp = OTHER,
								std::string_view content_type = ContentType::APPL_JSON, 
								std::string_view allow_method = "" );

FileResponse MakeFileResponse(http::status status, std::string_view file_name, unsigned http_version, std::string_view content_type);

// } // namespace http_server
..... ...FFF

{"action":"add_book","payload":{"title":"The Old Man and the Sea'); DROP TABLE books; --","author":"Hemingway","year":1952,"ISBN":"555"}}
