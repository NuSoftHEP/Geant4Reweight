cet_make_library( LIBRARY_NAME FitterBaseLib 
  SOURCE 
    G4ReweightFitter.cc
    DUETFitter.cc
    G4ReweightFitManager.cc
    G4ReweightPionFitManager.cc
  LIBRARIES
    fhiclcpp::fhiclcpp
    cetlib::cetlib
    cetlib_except::cetlib_except
    ${ROOT_BASIC_LIB_LIST}
    PropBaseLib
    ReweightBaseLib
)
install_headers()
install_source()
install_fhicl()
