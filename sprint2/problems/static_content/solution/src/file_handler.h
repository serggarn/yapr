#pragma once
#include <filesystem>
#include <boost/beast.hpp>
#include "http_types.h"
#include <map>

namespace file_handler {
namespace fs = std::filesystem;
namespace beast = boost::beast;
namespace http = beast::http;
using namespace http_types;

const std::map<std::string_view, std::string_view> extContType = {
	{ ".htm"sv,  ContentType::TEXT_HTML },
	{ ".html"sv, ContentType::TEXT_HTML },
	{ ".css"sv,  ContentType::TEXT_CSS },
	{ ".txt"sv,  ContentType::TEXT_PLAIN },
	{ ".js"sv,   ContentType::TEXT_JS },
	{ ".json"sv, ContentType::APPL_JSON },
	{ ".xml"sv,  ContentType::APPL_XML },
	{ ".png"sv,  ContentType::IMG_PNG },
	{ ".jpg"sv,  ContentType::IMG_JPG },
	{ ".jpe"sv,  ContentType::IMG_JPG },
	{ ".jpeg"sv, ContentType::IMG_JPG },
	{ ".gif"sv,  ContentType::IMG_GIF },
	{ ".bmp"sv,  ContentType::IMG_BMP },
	{ ".ico"sv,  ContentType::IMG_ICO },
	{ ".tiff"sv, ContentType::IMG_TIFF },
	{ ".tif"sv,  ContentType::IMG_TIFF },	
	{ ".svg"sv,  ContentType::IMG_SVG },
	{ ".svgz"sv, ContentType::IMG_SVG },
	{ ".mp3"sv,  ContentType::AUDIO_MP3 },
};


class Files {
public:
	explicit Files(std::string_view _base_path);
	// Функция проверят существование файла и возвращает абсолютный путь
	std::pair<http::status, std::string> GetPath(std::string_view path) const;
	// Возвращает true, если каталог p содержится внутри base.
	bool IsSubPath(fs::path path, fs::path base) const;
	std::string_view GetContentType(std::string_view path);
	
private:
	fs::path base_path;
};

}
