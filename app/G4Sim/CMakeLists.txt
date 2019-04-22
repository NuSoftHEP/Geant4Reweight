SET(G4SIM_APPS
  G4Sim
)

include_directories(${INCDIRS})

include(${Geant4_USE_FILE})
include_directories(${ROOT_INCLUDE_DIRS})

foreach(appname ${G4SIM_APPS})
  add_executable(${appname} ${appname}.cc)
  target_link_libraries(${appname} ${MODULETargets})
  target_link_libraries(${appname} ${ROOT_LIBRARIES})
  target_link_libraries(${appname} ${Geant4_LIBRARIES})

  install(TARGETS ${appname} DESTINATION bin)
endforeach()
