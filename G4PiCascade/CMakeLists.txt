#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(G4PiCascade)

include(ExternalProject)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()


list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO Net)

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})

#Importing fhiclcpp

message( "Use fhicl?" ${USE_FHICL} )
include(${CMAKE_SOURCE_DIR}/cmake/fhiclcppSetup.cmake)

include_directories(${ROOT_INCLUDE_DIRS} ${CMAKE_INSTALL_PREFIX}/include ${PROJECT_SOURCE_DIR}/include)


#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)


#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(G4PiCascade G4PiCascade.cc ${sources} ${headers})

if(DEFINED USE_FHICL AND USE_FHICL)
  message( "Adding FHICL")
  add_dependencies(G4PiCascade fhiclcpp-simple)
endif()

target_link_libraries(G4PiCascade ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})


install(TARGETS G4PiCascade DESTINATION bin)


