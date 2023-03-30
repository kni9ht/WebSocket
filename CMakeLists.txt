cmake_minimum_required(VERSION 3.14)

# Set project name
project(websocket-example)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)

# Include FetchContent module
include(FetchContent)

# Set CMP0135 policy to NEW
cmake_policy(SET CMP0135 NEW)

# Find and download Boost
FetchContent_Declare(
  boost
  URL https://boostorg.jfrog.io/artifactory/main/release/1.77.0/source/boost_1_77_0.tar.gz
)
FetchContent_MakeAvailable(boost)

# Find and download OpenSSL
FetchContent_Declare(
  openssl
  URL https://www.openssl.org/source/openssl-3.0.0.tar.gz
)
FetchContent_MakeAvailable(openssl)

# Add server executable
add_executable(server server.cpp)
target_link_libraries(server PRIVATE Boost::system Boost::thread OpenSSL::SSL OpenSSL::Crypto)

# Add client executable
add_executable(client client.cpp)
target_link_libraries(client PRIVATE Boost::system Boost::thread OpenSSL::SSL OpenSSL::Crypto)