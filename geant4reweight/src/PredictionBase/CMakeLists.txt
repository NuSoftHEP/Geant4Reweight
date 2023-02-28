cet_make_library( LIBRARY_NAME PredictionBaseLib 
  SOURCE 
    G4CascadeDetectorConstruction.cc
    G4CascadePhysicsList.cc
    G4DecayHook.cc
  LIBRARIES PRIVATE
    fhiclcpp::fhiclcpp
    cetlib::cetlib
    cetlib_except::cetlib_except
    CLHEP::CLHEP
    Geant4::G4event
    Geant4::G4intercoms
    Geant4::G4interfaces
    Geant4::G4run
    Geant4::G4tracking
    Geant4::G4materials
    Geant4::G4geometry
    Geant4::G4global
    Geant4::G4persistency
    Geant4::G4physicslists
)
install_headers()
install_source()
