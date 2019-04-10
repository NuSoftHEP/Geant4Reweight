# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(Prop)

include(ExternalProject)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

##Importing ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO Net)

#Importing fhiclcpp

message( "Use fhicl?" ${USE_FHICL} )
include(${CMAKE_SOURCE_DIR}/cmake/fhiclcppSetup.cmake)

include_directories(${ROOT_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include ${UTIL_INCLUDE_DIR} ${CMAKE_INSTALL_PREFIX}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)


add_library(PropLib SHARED ${sources}) 
add_executable(parMaker parMaker.cc ${sources} ${headers})

if(DEFINED USE_FHICL AND USE_FHICL)
  add_dependencies(parMaker fhiclcpp-simple)
  add_dependencies(PropLib fhiclcpp-simple)
endif()

target_link_libraries(parMaker ${ROOT_LIBRARIES})
target_link_libraries(PropLib  ${ROOT_LIBRARIES})

install(TARGETS parMaker DESTINATION bin)
install(TARGETS PropLib DESTINATION lib)

