SET(PROP_APPS
  parMaker 
)


foreach(appname ${PROP_APPS})
  cet_make_exec(NAME ${appname}
    SOURCE 
      ${appname}.cc
    LIBRARIES PRIVATE
      fhiclcpp::fhiclcpp
      cetlib::cetlib
      cetlib_except::cetlib_except
      geant4reweight::PropBaseLib
  )

endforeach()
