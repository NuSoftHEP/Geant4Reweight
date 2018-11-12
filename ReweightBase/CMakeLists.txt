# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(G4Reweight)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

##Importing ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO Net)

##Importing tinyxml2
#set(TINYXML2_INCLUDE_DIRS $ENV{TINYXML2_INCLUDE_DIRS})

include_directories(${ROOT_INCLUDE_DIRS} $ENV{TINYXML2_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(G4Reweight G4Reweight.cc ${sources} ${headers})

file(GLOB TINYXML2_LIBRARIES $ENV{TINYXML2_LIBRARY_DIR}/*.so*)

target_link_libraries(G4Reweight ${ROOT_LIBRARIES} ${TINYXML2_LIBRARIES})

install(TARGETS G4Reweight DESTINATION bin)

#add_library(G4Reweight ${PROJECT_SOURCE_DIR}/src/G4ReweightTraj.cc ${PROJECT_SOURCE_DIR}/include/G4ReweightTraj.hh)




