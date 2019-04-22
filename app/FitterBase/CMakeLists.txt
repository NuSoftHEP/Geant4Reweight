SET(FITTER_APPS
 Fitter 
)

if(DEFINED USE_FHICL AND USE_FHICL)
  add_dependencies(FitterBaseLib fhiclcpp-simple)
endif()

include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${INCDIRS})

# Requried for fhiclcpp
include_directories(${CMAKE_INSTALL_PREFIX}/include)

foreach(appname ${FITTER_APPS})
  add_executable(${appname} ${appname}.cc)
  target_link_libraries(${appname} ${MODULETargets})
  target_link_libraries(${appname} ${ROOT_LIBRARIES})

  install(TARGETS ${appname} DESTINATION bin)
endforeach()
