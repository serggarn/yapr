#include <iostream>
#include "file_handler.h"
#include <sstream>
#include <algorithm>
#include "encode.h"

namespace file_handler {

Files::Files(std::string_view _base_path)
{
	base_path = fs::absolute(fs::path{_base_path});
	if ( not fs::is_directory(base_path) ) {
		std::stringstream error_msg;
		error_msg << "Diriectory " << base_path << " not found";
		throw std::runtime_error(error_msg.str());
	}
	
}
using namespace std::literals;
std::pair<http::status, std::string>  Files::GetPath(std::string_view path) const
{
	std::pair<http::status, std::string> res { http::status::ok, ""sv };
	std::string normal_path = decodeURIComponent(path);
	std::cout << "normal path: " << normal_path<<":" << path <<std::endl;
	fs::path abs_path = fs::weakly_canonical(fs::path{base_path / fs::path{normal_path}.relative_path()});
	std::cout << "paths: " << base_path << "; " << abs_path << std::endl; // Выведет "/this/is/another/path"	
	if ( not IsSubPath(abs_path, base_path) ) {
		res.first = http::status::bad_request;
		return res;
	}
	if ( fs::is_directory(abs_path) )
		abs_path /= fs::path{defaultFile};
	std::cout << "issub ok " << abs_path.c_str() <<std::endl;
	if ( not fs::exists(abs_path) )
		res.first = http::status::not_found;
	res.second = abs_path.c_str();
	return res;
}

bool Files::IsSubPath(fs::path path, fs::path base) const {
    // Приводим оба пути к каноничному виду (без . и ..)
    path = fs::weakly_canonical(path);
    base = fs::weakly_canonical(base);

    // Проверяем, что все компоненты base содержатся внутри path
    for (auto b = base.begin(), p = path.begin(); b != base.end(); ++b, ++p) {
        if (p == path.end() || *p != *b) {
            return false;
        }
    }
    return true;
}

std::string_view Files::GetContentType(std::string_view path) {
	fs::path _path(path);
// 	std::string_view ext = 
	std::string ext = _path.extension().string();
	std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
	std::cout << " ext: " << ext << std::endl;
	auto result = extContType.find(ext);
	if ( result == extContType.end() )
		return http_types::ContentType::OTHER_TYPES;
	return result->second;
}
}
