cet_make_library(LIBRARY_NAME FitterBaseLib
  SOURCE 
  G4ReweightFitter.cc
  DUETFitter.cc
  G4ReweightFitManager.cc
  G4ReweightPionFitManager.cc
  LIBRARIES
  PUBLIC
  geant4reweight::PropBaseLib
  fhiclcpp::fhiclcpp
  ROOT::MathCore
  ROOT::Tree
  PRIVATE
  geant4reweight::ReweightBaseLib
  ROOT::Hist
  ROOT::Gpad
)

install_headers()
install_source()
install_fhicl()
