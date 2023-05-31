cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

# Сгенерируем конфигурацию, если нет
if (NOT EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
	message(STATUS, "conan install... ")
	# 	execute_process(COMMAND cd ${CMAKE_BINARY_DIR})
	execute_process(COMMAND conan install .. -s build_type=Debug  -s compiler.libcxx=libstdc++11)
	execute_process(COMMAND conan install .. -s build_type=Release -s compiler.libcxx=libstdc++11)
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(game_model STATIC
	src/geom.h
	src/model_serialization.h
	src/model.h
	src/model.cpp
	src/tagged.h
)

target_link_libraries(game_model PUBLIC CONAN_PKG::boost Threads::Threads)

add_executable(game_server_tests
	tests/state-serialization-tests.cpp
)

target_link_libraries(game_server_tests CONAN_PKG::catch2 game_model)
