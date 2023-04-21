cmake_minimum_required(VERSION 3.16)

project(HelloLog CXX)
set(CMAKE_CXX_STANDARD 20)

# add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)

# Сгенерируем конфигурацию, если нет
if (NOT EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	message(STATUS, "conan install... ")
# 	execute_process(COMMAND cd ${CMAKE_BINARY_DIR})
	exec_program(conan ${CMAKE_BINARY_DIR} ARGS install .. -s compiler.libcxx=libstdc++11 -s build_type=Debug)
	set(CMAKE_BUILD_TYPE Debug)
endif()

# обратите внимание на аргумент TARGETS у команды conan_basic_setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(hello_log main.cpp)

# используем "импортированную" цель CONAN_PKG::boost
target_include_directories(hello_log PRIVATE CONAN_PKG::boost)
target_link_libraries(hello_log CONAN_PKG::boost) 

