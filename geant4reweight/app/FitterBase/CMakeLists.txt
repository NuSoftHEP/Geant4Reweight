SET(FITTER_APPS
 Fitter 
)

foreach(appname ${FITTER_APPS})
  cet_make_exec( ${appname} 
    SOURCE 
      ${appname}.cc
    LIBRARIES
      fhiclcpp::fhiclcpp
      cetlib::cetlib
      cetlib_except::cetlib_except
      ${ROOT_BASIC_LIB_LIST}
      FitterBaseLib
      ReweightBaseLib
      PropBaseLib    
  )

endforeach()
