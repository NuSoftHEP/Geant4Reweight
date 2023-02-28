cet_make_library( LIBRARY_NAME FitterBaseLib 
  SOURCE 
    G4ReweightFitter.cc
    DUETFitter.cc
    G4ReweightFitManager.cc
    G4ReweightPionFitManager.cc
  LIBRARIES PUBLIC
    geant4reweight::PropBaseLib
    geant4reweight::ReweightBaseLib
  LIBRARIES PRIVATE
    fhiclcpp::fhiclcpp
    cetlib::cetlib
    cetlib_except::cetlib_except
)
install_headers()
install_source()
install_fhicl()
