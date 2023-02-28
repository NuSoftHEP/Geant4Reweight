cet_make_library( LIBRARY_NAME ReweightBaseLib 
  SOURCE 
    G4ReweightStep.cc
    G4ReweightTraj.cc
    G4ReweightManager.cc
    G4Reweighter.cc
    G4PiPlusReweighter.cc
    G4PiMinusReweighter.cc
    G4KPlusReweighter.cc
    G4KMinusReweighter.cc
    G4ProtonReweighter.cc
    G4NeutronReweighter.cc
    G4ReweighterFactory.cc
    G4MultiReweighter.cc
  LIBRARIES PUBLIC
    geant4reweight::PropBaseLib
    geant4reweight::PredictionBaseLib
    Geant4::G4event
    Geant4::G4intercoms
    Geant4::G4interfaces
    Geant4::G4run
    Geant4::G4track
    Geant4::G4tracking
    Geant4::G4particles
    Geant4::G4processes
    Geant4::G4materials
    Geant4::G4geometry
    Geant4::G4global
    Geant4::G4persistency
    Geant4::G4physicslists
  LIBRARIES PRIVATE
    fhiclcpp::fhiclcpp
    cetlib::cetlib
    cetlib_except::cetlib_except
    CLHEP::CLHEP
)
install_headers()
install_source()
