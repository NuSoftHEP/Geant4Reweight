#if(USEART)
  cet_make_library( LIBRARY_NAME PredictionBaseLib 
    SOURCE 
      G4CascadeDetectorConstruction.cc
      G4CascadePhysicsList.cc
    LIBRARIES
      ${FHICLCPP}
      #cetlib cetlib_except
      ${CETLIB} ${CETLIB_EXCEPT}
      clhep
      #CLHEP
      ${G4EVENT}
      ${G4INTERCOMS}
      ${G4INTERFACES}
      ${G4RUN}
      ${G4TRACKING}
      ${G4MATERIALS}
      ${G4GEOMETRY}
      ${G4GLOBAL}
      ${G4PERSISTENCY}
      ${G4PHYSICSLISTS}
      ${ROOT_BASIC_LIB_LIST}
  )
  install_headers()
  install_source()
#else()

#  file(GLOB sources *.cc)
#  add_library(PredictionBaseLib
#    ${sources}
#  )
#  
#  #include(${Geant4_USE_FILE})
#  
#  include_directories(${ROOT_INCLUDE_DIRS})
#  
#  include_directories(${INCDIRS})
#  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#  
#  include_directories(${FHICL_INCLUDE_DIR})
#  
#  #target_link_libraries(PredictionBaseLib ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
#  target_link_libraries(PredictionBaseLib ${CETLIB} ${CETLIB_EXCEPT} ${CLHEP}
#    ${G4EVENT} ${G4INTERCOMS} ${G4INTERFACES} ${G4RUN} ${G4TRACKING}
#    ${G4MATERIALS} ${G4GEOMETRY} ${G4GLOBAL} ${G4PERSISTENCY}
#    ${G4PHYSICSLISTS} ${ROOT_BASIC_LIB_LIST} #${ROOT_LIBRARIES}
#  )
#  
#  SET(MODULETargets ${MODULETargets};PredictionBaseLib PARENT_SCOPE)
#
#endif()
