cet_make_library(LIBRARY_NAME PredictionBaseLib
  SOURCE 
  G4CascadeDetectorConstruction.cc
  G4CascadePhysicsList.cc
  G4DecayHook.cc
  LIBRARIES
  PUBLIC
  Geant4::G4run
  Geant4::G4processes
  fhiclcpp::fhiclcpp
  PRIVATE
  Geant4::G4physicslists
)

install_headers()
install_source()
