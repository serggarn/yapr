//
// Created by serg on 23.04.23.
//
#pragma once

#include <boost/beast/http.hpp>
#include "http_types.h"

//class request {
namespace beast = boost::beast;
namespace http = beast::http;
namespace sys = boost::system;

using StringRequest = http::request<http::string_body>;

using namespace http_types;

//};


