art_make_library( LIBRARY_NAME ReweightBaseLib 
  SOURCE 
    G4ReweightStep.cc
    G4ReweightTraj.cc
    G4Reweighter.cc
    G4PiPlusReweighter.cc
    G4PiMinusReweighter.cc
    G4ProtonReweighter.cc
    G4ReweighterFactory.cc
  LIBRARIES
    ${ART_FRAMEWORK_CORE}
    ${ART_FRAMEWORK_PRINCIPAL}
    ${ART_FRAMEWORK_SERVICES_REGISTRY}
    ${ART_FRAMEWORK_SERVICES_BASIC}
    canvas
    ${MF_MESSAGELOGGER}
    ${FHICLCPP}
    cetlib cetlib_except
    ${ROOT_BASIC_LIB_LIST}
)
install_headers()
install_source()
