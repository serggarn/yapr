cmake_minimum_required(VERSION 3.16)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

if (EXISTS /usr/bin/clang-12)
	set(CMAKE_C_COMPILER /usr/bin/clang-12)
	set(CMAKE_CXX_COMPILER /usr/bin/clang++-12)
endif()

# Сгенерируем конфигурацию, если нет
if (NOT EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
	message(STATUS, "conan install... ")
# 	execute_process(COMMAND cd ${CMAKE_BINARY_DIR})
	execute_process(COMMAND conan install .. -s build_type=Debug  -s compiler.libcxx=libstdc++11)
	execute_process(COMMAND conan install .. -s build_type=Release -s compiler.libcxx=libstdc++11)
endif()

set(CMAKE_BUILD_TYPE Release)

# обратите внимание на аргумент TARGETS у команды conan_basic_setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)
add_compile_definitions(BOOST_JSON_USE_STD_STRING_VIEW)

# find_package(Boost 1.78.0 REQUIRED)
# if(Boost_FOUND)
#   include_directories(${Boost_INCLUDE_DIRS})
# endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(PROJECT_DIR .)

file(GLOB_RECURSE PROJECT_SOURCES
		${PROJECT_DIR}/src/app_layer/*.cpp
#		${PROJECT_DIR}/src/collision/*.cpp
		${PROJECT_DIR}/src/game_play/*.cpp
		${PROJECT_DIR}/src/json/*.cpp
		${PROJECT_DIR}/src/system/*.cpp
		${PROJECT_DIR}/src/*.cpp
		)

SET (PROJECT_HEADERS
		${PROJECT_DIR}/src/app_layer/*.h
#		${PROJECT_DIR}/src/collision/*.h
		${PROJECT_DIR}/src/game_play/*.h
		${PROJECT_DIR}/src/json/*.h
		${PROJECT_DIR}/src/system/*.h
		${PROJECT_DIR}/src/*.h
		)

# Добавляем библиотеку, указывая, что она статическая.
add_library(ModelLib STATIC
		src/model/model.h
		src/model/model.cpp
		src/model/dog.h
		src/model/dog.cpp
		src/model/player.h
		src/model/player.cpp
		src/model/players.h
		src/model/players.cpp
		src/tagged.h
		src/model/loot_generator.h
		src/model/loot_generator.cpp)

add_library(collision_detection_lib STATIC
		src/collision/collision_detector.h
		src/collision/collision_detector.cpp
		)

target_link_libraries(collision_detection_lib PUBLIC CONAN_PKG::boost Threads::Threads)

add_executable(collision_detection_tests
		tests/collision-detector-tests.cpp
		)

target_link_libraries(collision_detection_tests CONAN_PKG::catch2 collision_detection_lib)

add_executable(catch_loot_generator_tests
		tests/loot_generator_tests.cpp
		src/model/loot_generator.h
		src/model/loot_generator.cpp
		)

add_executable(state_serialization_tests
		tests/state-serialization-tests.cpp
		src/model/model.h
		src/model/model.cpp
		src/model/model_serialization.h
		src/system/backup.h
		src/system/backup.cpp
		)
target_link_libraries(state_serialization_tests CONAN_PKG::catch2 CONAN_PKG::boost)

add_executable(game_server ${PROJECT_SOURCES})



target_link_libraries(ModelLib PUBLIC CONAN_PKG::boost)

# используем "импортированную" цель CONAN_PKG::boost
#target_include_directories(game_server PRIVATE CONAN_PKG::boost)
# target_link_libraries(game_server PRIVATE Threads::Threads)
#target_link_libraries(game_server CONAN_PKG::boost)

target_link_libraries(game_server ModelLib collision_detection_lib)
target_include_directories(game_server PRIVATE ${PROJECT_HEADERS})
#target_link_libraries(catch_loot_generator_tests ModelLib)
target_link_libraries(catch_loot_generator_tests PRIVATE CONAN_PKG::catch2 ModelLib)
