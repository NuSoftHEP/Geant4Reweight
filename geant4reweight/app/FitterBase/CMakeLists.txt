SET(FITTER_APPS
 Fitter 
)

foreach(appname ${FITTER_APPS})
  cet_make_exec( ${appname} 
    SOURCE 
      ${appname}.cc
    LIBRARIES
      ${FHICLCPP}
      ${CETLIB} ${CETLIB_EXCEPT}
      ${ROOT_BASIC_LIB_LIST}
      FitterBaseLib
      ReweightBaseLib
      PropBaseLib    
  )

endforeach()
