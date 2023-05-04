cmake_minimum_required(VERSION 3.16)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

if (EXISTS /usr/bin/clang-12)
	set(CMAKE_C_COMPILER /usr/bin/clang-12)
	set(CMAKE_CXX_COMPILER /usr/bin/clang++-12)
endif()

# Сгенерируем конфигурацию, если нет
if (NOT EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	message(STATUS, "conan install... ")
# 	execute_process(COMMAND cd ${CMAKE_BINARY_DIR})
	exec_program(conan ${CMAKE_BINARY_DIR} ARGS install .. -s compiler.libcxx=libstdc++11 -s build_type=Release)
	set(CMAKE_BUILD_TYPE Release)
endif()

# обратите внимание на аргумент TARGETS у команды conan_basic_setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)
add_compile_definitions(BOOST_JSON_USE_STD_STRING_VIEW)

# find_package(Boost 1.78.0 REQUIRED)
# if(Boost_FOUND)
#   include_directories(${Boost_INCLUDE_DIRS})
# endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(game_server
	src/main.cpp
	src/http_server.cpp
	src/http_server.h
	src/sdk.h
	src/model.h
	src/model.cpp
	src/tagged.h
	src/boost_json.cpp
	src/json_loader.h
	src/json_loader.cpp
	src/request_handler.cpp
	src/request_handler.h
	src/api_handler.cpp
	src/api_handler.h
	src/file_handler.cpp
	src/file_handler.h
	src/http_types.h
	src/encode.h
	src/logger.cpp
	src/logger.h
	src/response.cpp
	src/response.h
	src/game_session.h
	src/game_session.cpp
	src/dog.h
	src/dog.cpp
	src/player.h
	src/players.h
	src/players.cpp
	src/game.h
	src/game.cpp
	src/token.h
        src/player.cpp)

# используем "импортированную" цель CONAN_PKG::boost
target_include_directories(game_server PRIVATE CONAN_PKG::boost)
# target_link_libraries(game_server PRIVATE Threads::Threads)
target_link_libraries(game_server CONAN_PKG::boost) 


