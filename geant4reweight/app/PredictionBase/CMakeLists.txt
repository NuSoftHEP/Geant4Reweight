cet_make_exec(NAME G4Cascade
  LIBRARIES PRIVATE
  geant4reweight::PredictionBaseLib
  Geant4::G4global
  Geant4::G4materials
  Geant4::G4particles
  Geant4::G4processes
  Geant4::G4run
  Geant4::G4track
  ROOT::Hist
  ROOT::Matrix
  ROOT::RIO
  ROOT::Tree
  fhiclcpp::fhiclcpp
  cetlib::cetlib
)

cet_make_exec(NAME G4CrossSection
  LIBRARIES PRIVATE
  geant4reweight::PredictionBaseLib
  Geant4::G4geometry
  Geant4::G4global
  Geant4::G4materials
  Geant4::G4particles
  Geant4::G4processes
  Geant4::G4run
  Geant4::G4track
  ROOT::Hist
  ROOT::Matrix
  ROOT::RIO
  ROOT::Tree
  fhiclcpp::fhiclcpp
  cetlib::cetlib
)

cet_make_exec(NAME G4Elastic
  LIBRARIES PRIVATE
  geant4reweight::PredictionBaseLib
  Geant4::G4global
  Geant4::G4materials
  Geant4::G4particles
  Geant4::G4processes
  Geant4::G4run
  Geant4::G4track
  Geant4::G4tracking
  ROOT::Matrix
  ROOT::RIO
  ROOT::Tree
  fhiclcpp::fhiclcpp
  cetlib::cetlib
)

if (WANT_G4DDCrossSection)
  cet_make_exec(NAME G4DDCrossSection
    LIBRARIES PRIVATE
    geant4reweight::PredictionBaseLib
    Geant4::G4global
    Geant4::G4materials
    Geant4::G4particles
    Geant4::G4processes
    Geant4::G4run
    Geant4::G4track
    Geant4::G4tracking
    ROOT::Hist
    ROOT::MathCore
    ROOT::Matrix
    ROOT::RIO
    fhiclcpp::fhiclcpp
    cetlib::cetlib
  )
endif()

install_fhicl()
