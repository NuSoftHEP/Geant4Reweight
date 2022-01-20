SET(PREDICTION_APPS
  G4CrossSection
  G4Cascade
  G4Elastic
)


foreach(appname ${PREDICTION_APPS})
  cet_make_exec( ${appname} 
    SOURCE 
      ${appname}.cc
    LIBRARIES
      fhiclcpp::fhiclcpp
      cetlib::cetlib
      cetlib_except::cetlib_except
      CLHEP::CLHEP
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
      ${ROOT_BASIC_LIB_LIST}
      PredictionBaseLib
  )

endforeach()

install_fhicl()
