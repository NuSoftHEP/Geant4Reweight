# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(G4Reweight)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO Net)

include_directories(${ROOT_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(G4Reweight G4Reweight.cc ${sources} ${headers})
target_link_libraries(G4Reweight ${ROOT_LIBRARIES})

install(TARGETS G4Reweight DESTINATION bin)

#add_library(G4Reweight ${PROJECT_SOURCE_DIR}/src/G4ReweightTraj.cc ${PROJECT_SOURCE_DIR}/include/G4ReweightTraj.hh)




