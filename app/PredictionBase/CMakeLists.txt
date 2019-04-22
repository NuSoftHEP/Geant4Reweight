SET(PREDICTION_APPS
  G4CrossSection
  G4PiCascade
)

if(DEFINED USE_FHICL AND USE_FHICL)
  add_dependencies(PredictionBaseLib fhiclcpp-simple)
endif()

include(${Geant4_USE_FILE})
include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${INCDIRS})

# Requried for fhiclcpp
include_directories(${CMAKE_INSTALL_PREFIX}/include)

foreach(appname ${PREDICTION_APPS})
  add_executable(${appname} ${appname}.cc)
  target_link_libraries(${appname} ${MODULETargets})
  target_link_libraries(${appname} ${ROOT_LIBRARIES})
  target_link_libraries(${appname} ${Geant4_LIBRARIES})

  install(TARGETS ${appname} DESTINATION bin)
endforeach()
