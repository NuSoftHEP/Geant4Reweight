cet_make_library( LIBRARY_NAME PropBaseLib 
  SOURCE 
    G4ReweightParameterMaker.cc
    G4ReweightThrowManager.cc
  LIBRARIES PRIVATE
    fhiclcpp::fhiclcpp
    cetlib::cetlib
    cetlib_except::cetlib_except
  LIBRARIES PUBLIC
    ROOT::Tree
    ROOT::Gpad
    ROOT::Hist
    ROOT::Core
)
install_headers()
install_source()
