SET(PREDICTION_APPS
  G4CrossSection
  G4Cascade
)
#
#if(DEFINED USE_FHICL AND USE_FHICL)
#  add_dependencies(PredictionBaseLib fhiclcpp-simple)
#else()
#  include_directories(${CETLIB_EXCEPT_INCLUDE_DIR})
#  include_directories(${CETLIB_INCLUDE_DIR})
#endif()
#
#include(${Geant4_USE_FILE})
#include_directories(${ROOT_INCLUDE_DIRS})
#include_directories(${INCDIRS})
#
## Requried for fhiclcpp
##if(DEFINED USE_FHICL AND USE_FHICL)
##include_directories(${CMAKE_INSTALL_PREFIX}/include)
#include_directories(${FHICL_INCLUDE_DIR})
##endif()
#
#foreach(appname ${PREDICTION_APPS})
#  add_executable(${appname} ${appname}.cc)
#  target_link_libraries(${appname} ${MODULETargets})
#  target_link_libraries(${appname} ${ROOT_LIBRARIES})
#  target_link_libraries(${appname} ${Geant4_LIBRARIES})
#
#  if(NOT DEFINED USE_FHICL OR NOT USE_FHICL)
#    target_link_libraries(${appname} ${FHICLCPP_LIBS}) 
#  endif()
#
#  install(TARGETS ${appname} DESTINATION bin)
#endforeach()


foreach(appname ${PREDICTION_APPS})
  art_make_exec( NAME ${appname} 
    SOURCE 
      ${appname}.cc
    LIBRARIES
      ${ART_FRAMEWORK_CORE}
      ${ART_FRAMEWORK_PRINCIPAL}
      ${ART_FRAMEWORK_SERVICES_REGISTRY}
      ${ART_FRAMEWORK_SERVICES_BASIC}
      canvas
      ${MF_MESSAGELOGGER}
      ${FHICLCPP}
      cetlib cetlib_except
      clhep
      ${G4EVENT}
      ${G4INTERCOMS}
      ${G4INTERFACES}
      ${G4RUN}
      ${G4TRACK}
      ${G4TRACKING}
      ${G4PARTICLES}
      ${G4PROCESSES}
      ${G4MATERIALS}
      ${G4GEOMETRY}
      ${G4GLOBAL}
      ${G4PERSISTENCY}
      ${G4PHYSICSLISTS}
      ${ROOT_BASIC_LIB_LIST}
      PredictionBaseLib    
  )

endforeach()
