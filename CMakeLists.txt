cmake_minimum_required(VERSION 3.10)

project(Server)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system thread)

add_executable(Server Server.cpp)

add_executable(Client Client.cpp)


target_link_libraries(Server PRIVATE Boost::system Boost::thread)
target_link_libraries(Client PRIVATE Boost::system Boost::thread)