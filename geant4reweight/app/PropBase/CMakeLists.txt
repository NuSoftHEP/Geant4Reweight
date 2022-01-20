SET(PROP_APPS
  parMaker 
)


foreach(appname ${PROP_APPS})
  cet_make_exec( ${appname} 
    SOURCE 
      ${appname}.cc
    LIBRARIES
      fhiclcpp::fhiclcpp
      cetlib::cetlib
      cetlib_except::cetlib_except
      ${ROOT_BASIC_LIB_LIST}
      PropBaseLib    
  )

endforeach()
