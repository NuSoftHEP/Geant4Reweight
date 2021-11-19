cet_make_library( LIBRARY_NAME PredictionBaseLib 
  SOURCE 
    G4CascadeDetectorConstruction.cc
    G4CascadePhysicsList.cc
    G4DecayHook.cc
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
