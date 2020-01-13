#if(USEART)
  cet_make_library( LIBRARY_NAME PropBaseLib 
    SOURCE 
      G4ReweightParameterMaker.cc
      G4ReweightThrowManager.cc
    LIBRARIES
      ${FHICLCPP}
      ${CETLIB} ${CETLIB_EXCEPT}
      ${ROOT_BASIC_LIB_LIST}
  )
  install_headers()
  install_source()
#else()
#  file(GLOB sources *.cc)
#  add_library(PropBaseLib
#    ${sources}
#  )
#  
#  include_directories(${ROOT_INCLUDE_DIRS})
#  
#  include_directories(${CETLIB_EXCEPT_INCLUDE_DIR})
#  include_directories(${CETLIB_INCLUDE_DIR})
#
#  include_directories(${INCDIRS})
#  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#
#
#  
#  # Requried for fhiclcpp
#  #include_directories(${CMAKE_INSTALL_PREFIX}/include)
#  include_directories(${FHICL_INCLUDE_DIR})
#  
#  install(TARGETS PropBaseLib DESTINATION lib)
#  
#  target_link_libraries(PropBaseLib ${CETLIB} ${CETLIB_EXCEPT} ${ROOT_LIBRARIES})
#  #if(NOT DEFINED USE_FHICL OR NOT USE_FHICL)
#    target_link_libraries(PropBaseLib ${FHICLCPP_LIBS}) 
#  #endif()
#  
#  SET(MODULETargets ${MODULETargets};PropBaseLib PARENT_SCOPE)


#endif()
