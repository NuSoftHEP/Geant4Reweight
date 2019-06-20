# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(GeantReweight)

include(ExternalProject)

if(NOT "${CMAKE_CXX_STANDARD}")
  set(CMAKE_CXX_STANDARD 17)
endif()

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

###############################  fhiclcpp  #####################################
if(DEFINED USE_FHICL AND USE_FHICL)
  include(${CMAKE_SOURCE_DIR}/cmake/fhiclcppSetup.cmake)
  set(FHICL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include) 
else()
  set(FHICL_INCLUDE_DIR $ENV{FHICLCPP_INC})
  set(CETLIB_EXCEPT_INCLUDE_DIR $ENV{CETLIB_EXCEPT_INC})
  set(CETLIB_INCLUDE_DIR $ENV{CETLIB_INC})
  file(GLOB FHICLCPP_LIBS $ENV{FHICLCPP_LIB}/*.so)

  #add_compile_definitions(FNAL_FHICL)
  add_definitions(-DFNAL_FHICL)
endif()



add_subdirectory(src)
add_subdirectory(app)
