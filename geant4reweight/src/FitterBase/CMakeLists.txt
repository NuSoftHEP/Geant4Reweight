#if(USEART)
  cet_make_library( LIBRARY_NAME FitterBaseLib 
    SOURCE 
      G4ReweightFitter.cc
      DUETFitter.cc
      G4ReweightFitManager.cc
    LIBRARIES
      ${FHICLCPP}
      ${CETLIB} ${CETLIB_EXCEPT}
      ${ROOT_BASIC_LIB_LIST}
      PropBaseLib
      ReweightBaseLib
  )
  install_headers()
  install_source()
#else()
  #file(GLOB sources *.cc)
#  file(GLOB sources G4ReweightFitter.cc DUETFitter.cc G4ReweightFitManager.cc)
#  add_library(FitterBaseLib
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
#  # Requried for fhiclcpp
#  #include_directories(${CMAKE_INSTALL_PREFIX}/include)
#  include_directories(${FHICL_INCLUDE_DIR})
#  
#  install(TARGETS FitterBaseLib DESTINATION lib)
#  
#  #target_link_libraries(FitterBaseLib cetlib cetlib_except ${ROOT_LIBRARIES})
#  target_link_libraries(FitterBaseLib ${CETLIB} ${CETLIB_EXCEPT} ${ROOT_LIBRARIES})
#  
#  #if(NOT DEFINED USE_FHICL OR NOT USE_FHICL) 
#    target_link_libraries(FitterBaseLib ${FHICLCPP_LIBS}) 
#  #endif()
#
#  target_link_libraries(FitterBaseLib PropBaseLib ReweightBaseLib)
#  
#  SET(MODULETargets ${MODULETargets};FitterBaseLib PARENT_SCOPE)
#endif()
