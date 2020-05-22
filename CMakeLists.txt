cmake_minimum_required(VERSION 3.5)

project(http-redirect LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

add_executable(http-redirect
  main.cpp
  Redirector.h
  Redirector.cpp
)
target_link_libraries(http-redirect Qt5::Core Qt5::Network)
