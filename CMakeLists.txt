cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_COVERAGE "Build coverage" OFF)

set(
  HUNTER_CACHE_SERVERS
  "https://github.com/bmstu-iu8-cpp-sem-3/hunter-cache"
  CACHE STRING "Default cache server"
)

include("tools/gate/HunterGate.cmake")

HunterGate(
  URL "https://github.com/cpp-pm/hunter/archive/v0.23.218.tar.gz"
  SHA1 "9a3594a61227967fb92159339ba326701b287b19"
)


# TODO: rename project and delete this comment
project(template)
string(APPEND CMAKE_CXX_FLAGS " -pedantic -Werror -Wall -Wextra")
string(APPEND CMAKE_CXX_FLAGS " -Wno-unused-command-line-argument")
string(APPEND CMAKE_CXX_FLAGS " -Wshadow -Wnon-virtual-dtor")

hunter_add_package(Boost COMPONENTS filesystem log thread regex)
find_package(Boost CONFIG REQUIRED filesystem log log_setup thread regex)
find_package(Threads)

# TODO: change template word to project name and delete this comment
add_library(${PROJECT_NAME} STATIC
  # enum your files and delete this comment
        ${CMAKE_CURRENT_SOURCE_DIR}/sources/Client.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/sources/Server.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sources/Message.cpp)
add_executable(${PROJECT_NAME}
  # TODO: enum your files and delete this comment
  ${CMAKE_CURRENT_SOURCE_DIR}/sources/Log.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sources/main.cpp
)

# TODO: change template word to project name and delete this comment
target_include_directories(${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# TODO: change template word to project name and delete this comment
target_link_libraries(${PROJECT_NAME}
        ${CMAKE_THREAD_LIBS_INIT}
        Boost::log_setup
        Boost::log
        Boost::thread
        Boost::filesystem
        Boost::regex)

if(BUILD_COVERAGE)
  set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage build." FORCE)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  find_package(codecov)
  # TODO: change template word to project name and delete this comment
  add_coverage(${PROJECT_NAME})
  coverage_evaluate()
endif()
