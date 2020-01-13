SET(PREDICTION_APPS
  G4CrossSection
  G4Cascade
)


#if(USEART)
  foreach(appname ${PREDICTION_APPS})
    cet_make_exec( ${appname} 
      SOURCE 
        ${appname}.cc
      LIBRARIES
        ${FHICLCPP}
        ${CETLIB} ${CETLIB_EXCEPT}
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
#else()
#  include_directories(${CETLIB_EXCEPT_INCLUDE_DIR})
#  include_directories(${CETLIB_INCLUDE_DIR})
#  
#  #include(${Geant4_USE_FILE})
#  include_directories(${ROOT_INCLUDE_DIRS})
#  include_directories(${INCDIRS})
#  
#  # Requried for fhiclcpp
#  #if(DEFINED USE_FHICL AND USE_FHICL)
#  #include_directories(${CMAKE_INSTALL_PREFIX}/include)
#  include_directories(${FHICL_INCLUDE_DIR})
#  #endif()
#  
#  foreach(appname ${PREDICTION_APPS})
#    add_executable(${appname} ${appname}.cc)
#    target_link_libraries(${appname} ${MODULETargets})
#    target_link_libraries(${appname} ${ROOT_LIBRARIES})
#    #target_link_libraries(${appname} ${Geant4_LIBRARIES})
#  
#    #if(NOT DEFINED USE_FHICL OR NOT USE_FHICL)
#      target_link_libraries(${appname} ${FHICLCPP_LIBS}) 
#    #endif()
#
#    target_link_libraries(${appname} 
#      ${G4EVENT} ${G4INTERCOMS} ${G4INTERFACES}
#      ${G4RUN} ${G4TRACK} ${G4TRACKING} ${G4PARTICLES}
#      ${G4PROCESSES} ${G4MATERIALS} ${G4GEOMETRY}
#      ${G4GLOBAL} ${G4PERSISTENCY} ${G4PHYSICSLISTS}
#    )
#  
#    install(TARGETS ${appname} DESTINATION bin)
#  endforeach()

#endif()
