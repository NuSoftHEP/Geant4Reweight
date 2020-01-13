SET(FITTER_APPS
 Fitter 
)

#if(USEART)
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
#else()


#  include_directories(${CETLIB_EXCEPT_INCLUDE_DIR})
#  include_directories(${CETLIB_INCLUDE_DIR})
#  
#  include_directories(${ROOT_INCLUDE_DIRS})
#  include_directories(${INCDIRS})
#  
#  # Requried for fhiclcpp
#  #include_directories(${CMAKE_INSTALL_PREFIX}/include)
#  include_directories(${FHICL_INCLUDE_DIR})
#  
#  foreach(appname ${FITTER_APPS})
#    add_executable(${appname} ${appname}.cc)
#    target_link_libraries(${appname} ${FHICLCPP_LIBS} ${CETLIB} ${CETLIB_EXCEPT} ${ROOT_LIBRARIES} ${MODULETargets})
#    install(TARGETS ${appname} DESTINATION bin)
#  endforeach()
#endif()
