cmake_minimum_required(VERSION 3.28)

find_package(SystemCLanguage CONFIG REQUIRED)

target_link_libraries(test_systemc /usr/local/systemc-2.3.1)

#target_include_directories(/usr/local/systemc-2.3.1/include)

project(CA4B)

set(CMAKE_CXX_STANDARD 11)

add_executable(CA4B main.cpp
        components.hpp
)
