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
      ${FHICLCPP}
      ${CETLIB} ${CETLIB_EXCEPT}
      clhep
      ${G4EVENT}
      ${G4INTERCOMS}
      ${G4INTERFACES}
      ${G4RUN}
      ${G4TRACK}
      ${G4TRACKING}
      ${G4PARTICLES}
      ${G4PROCESSES}
      ${G4MATERIALS}
      ${G4GEOMETRY}
      ${G4GLOBAL}
      ${G4PERSISTENCY}
      ${G4PHYSICSLISTS}
      ${ROOT_BASIC_LIB_LIST}
      PredictionBaseLib    
  )

endforeach()

install_fhicl()
