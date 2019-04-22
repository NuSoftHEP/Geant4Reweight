SET(REWEIGHT_APPS
  G4Reweight
  G4ThrowReweight
)

if(DEFINED USE_FHICL AND USE_FHICL)
  add_dependencies(ReweightBaseLib fhiclcpp-simple)
endif()

include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${INCDIRS})

# Requried for fhiclcpp
include_directories(${CMAKE_INSTALL_PREFIX}/include)

foreach(appname ${REWEIGHT_APPS})
  add_executable(${appname} ${appname}.cc)
  target_link_libraries(${appname} ${MODULETargets})
  target_link_libraries(${appname} ${ROOT_LIBRARIES})

  install(TARGETS ${appname} DESTINATION bin)
endforeach()
