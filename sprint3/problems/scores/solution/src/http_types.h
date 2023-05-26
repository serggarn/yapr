#pragma once
#include <iostream>
using namespace std::literals;

namespace http_types {

// Структура ContentType задаёт область видимости для констант,
// задающий значения HTTP-заголовка Content-Type
struct ContentType {
	ContentType() = delete;
	constexpr static std::string_view TEXT_HTML = "text/html"sv;
	constexpr static std::string_view TEXT_CSS = "text/css"sv;
	constexpr static std::string_view TEXT_PLAIN = "text/plain"sv;
	constexpr static std::string_view TEXT_JS = "text/javascript"sv;
	constexpr static std::string_view APPL_JSON = "application/json"sv;
	constexpr static std::string_view APPL_XML = "application/xml"sv;
	constexpr static std::string_view IMG_PNG = "image/png"sv;
	constexpr static std::string_view IMG_JPG = "image/jpeg"sv;
	constexpr static std::string_view IMG_GIF = "image/gif"sv;
	constexpr static std::string_view IMG_BMP = "image/bmp"sv;
	constexpr static std::string_view IMG_ICO = "image/vnd.microsoft.icon"sv;
	constexpr static std::string_view IMG_TIFF = "image/tiff"sv;
	constexpr static std::string_view IMG_SVG = "image/svg+xml"sv;
	constexpr static std::string_view AUDIO_MP3 = "audio/mpeg"sv;
	constexpr static std::string_view OTHER_TYPES = "application/octet-stream"sv;
};

typedef enum : uint8_t {
	GET,
	HEAD,
	OTHER,
} requests_e;

struct UriType {
	UriType() = delete;
	constexpr static std::string_view URI_START = "/api/"sv;
	constexpr static std::string_view URI_MAPS = "/api/v1/maps"sv;
	constexpr static std::string_view URI_JOIN = "/api/v1/game/join"sv;
    constexpr static std::string_view URI_ACTION = "/api/v1/game/player/action"sv;
	constexpr static std::string_view URI_PLAYERS = "/api/v1/game/players"sv;
    constexpr static std::string_view URI_STATE = "/api/v1/game/state"sv;
    constexpr static std::string_view URI_TICK = "/api/v1/game/tick"sv;
};

}
