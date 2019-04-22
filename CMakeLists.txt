# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(GeantReweight)

include(ExternalProject)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


#################################  GEANT4  #####################################
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()


##################################  ROOT  ######################################
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO Net)

message(${ROOT_INCLUDE_DIRS})
message("Found ROOT" ${ROOT_FOUND})


###############################  fhiclcpp  #####################################
include(${CMAKE_SOURCE_DIR}/cmake/fhiclcppSetup.cmake)



add_subdirectory(src)
add_subdirectory(app)
