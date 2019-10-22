art_make_library( LIBRARY_NAME PredictionBaseLib 
  SOURCE 
    G4CascadeDetectorConstruction.cc
    G4CascadePhysicsList.cc
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
