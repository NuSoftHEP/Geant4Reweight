# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(Fitter)

include(ExternalProject)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

##Importing ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO Net)

##Importing Reweight
set(G4REWEIGHT_INCLUDE_DIR $ENV{G4REWEIGHT_INCLUDE_DIR})
file(GLOB G4REWEIGHT_LIBRARIES $ENV{G4REWEIGHT_LIBRARY_DIR}/*.so)

message( ${G4REWEIGHT_INCLUDE_DIR} ${G4REWEIGHT_LIBRARIES})


#Importing fhiclcpp

message( "Use fhicl?" ${USE_FHICL} )
include(${CMAKE_SOURCE_DIR}/cmake/fhiclcppSetup.cmake)

include_directories(${ROOT_INCLUDE_DIRS} ${G4REWEIGHT_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/include ${UTIL_INCLUDE_DIR} ${CMAKE_INSTALL_PREFIX}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)


add_executable(newFitter newFitter.cc ${sources} ${headers})
add_executable(curveFitter curveFitter.cc ${sources} ${headers})

if(DEFINED USE_FHICL AND USE_FHICL)
  add_dependencies(newFitter fhiclcpp-simple)
  add_dependencies(curveFitter fhiclcpp-simple)
endif()

#target_link_libraries(Fitter ${G4REWEIGHT_LIBRARIES} ${ROOT_LIBRARIES}     )
target_link_libraries(newFitter ${G4REWEIGHT_LIBRARIES} ${ROOT_LIBRARIES}   )
target_link_libraries(curveFitter ${G4REWEIGHT_LIBRARIES} ${ROOT_LIBRARIES} )

install(TARGETS newFitter DESTINATION bin)
install(TARGETS curveFitter DESTINATION bin)

