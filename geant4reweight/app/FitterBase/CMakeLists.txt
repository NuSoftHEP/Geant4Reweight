SET(FITTER_APPS
 Fitter 
)

foreach(appname ${FITTER_APPS})
  cet_make_exec( NAME ${appname}
    SOURCE 
      ${appname}.cc
    LIBRARIES PRIVATE
      fhiclcpp::fhiclcpp
      cetlib::cetlib
      cetlib_except::cetlib_except
      geant4reweight::FitterBaseLib
      geant4reweight::ReweightBaseLib
      geant4reweight::PropBaseLib
  )

endforeach()

install_fhicl()
