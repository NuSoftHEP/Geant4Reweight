cet_make_library( LIBRARY_NAME PropBaseLib 
  SOURCE 
    G4ReweightParameterMaker.cc
    G4ReweightThrowManager.cc
  LIBRARIES
    ${FHICLCPP}
    ${CETLIB} ${CETLIB_EXCEPT}
    ${ROOT_BASIC_LIB_LIST}
)
install_headers()
install_source()
