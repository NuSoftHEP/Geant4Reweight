SET(PROP_APPS
  parMaker 
)


foreach(appname ${PROP_APPS})
  cet_make_exec( ${appname} 
    SOURCE 
      ${appname}.cc
    LIBRARIES
      ${FHICLCPP}
      ${CETLIB} ${CETLIB_EXCEPT}
      ${ROOT_BASIC_LIB_LIST}
      PropBaseLib    
  )

endforeach()
