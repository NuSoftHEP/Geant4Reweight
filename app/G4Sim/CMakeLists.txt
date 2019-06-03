SET(G4SIM_APPS
  G4Sim
  G4Thin
)

if(DEFINED USE_FHICL AND USE_FHICL)
  add_dependencies(G4SimLib fhiclcpp-simple)
else()
  include_directories(${CETLIB_EXCEPT_INCLUDE_DIR})
  include_directories(${CETLIB_INCLUDE_DIR})
endif()

include_directories(${INCDIRS})

include(${Geant4_USE_FILE})
include_directories(${ROOT_INCLUDE_DIRS})

# Requried for fhiclcpp
#include_directories(${CMAKE_INSTALL_PREFIX}/include)
include_directories(${FHICL_INCLUDE_DIR})

foreach(appname ${G4SIM_APPS})
  add_executable(${appname} ${appname}.cc)
  target_link_libraries(${appname} ${MODULETargets})
  target_link_libraries(${appname} ${ROOT_LIBRARIES})
  target_link_libraries(${appname} ${Geant4_LIBRARIES})

  if(NOT DEFINED USE_FHICL OR NOT USE_FHICL)
    target_link_libraries(${appname} ${FHICLCPP_LIBS}) 
  endif()

  install(TARGETS ${appname} DESTINATION bin)
endforeach()
