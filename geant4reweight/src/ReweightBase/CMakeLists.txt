#if(USEART)
  cet_make_library( LIBRARY_NAME ReweightBaseLib 
    SOURCE 
      G4ReweightStep.cc
      G4ReweightTraj.cc
      G4Reweighter.cc
      G4PiPlusReweighter.cc
      G4PiMinusReweighter.cc
      G4ProtonReweighter.cc
      G4ReweighterFactory.cc
    LIBRARIES
      ${FHICLCPP}
      ${CETLIB} ${CETLIB_EXCEPT}
      ${ROOT_BASIC_LIB_LIST}
  )
  install_headers()
  install_source()
#else()
  #file(GLOB sources *.cc)
#  file(GLOB sources G4ReweightStep.cc G4ReweightTraj.cc G4Reweighter.cc 
#    G4PiPlusReweighter.cc G4PiMinusReweighter.cc G4ProtonReweighter.cc
#    G4ReweighterFactory.cc
#  )
#    
#  add_library(ReweightBaseLib
#    ${sources}
#  )
#  
#  include_directories(${ROOT_INCLUDE_DIRS})
#  
#  include_directories(${INCDIRS})
#  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#  
#  # Requried for fhiclcpp
#  #include_directories(${CMAKE_INSTALL_PREFIX}/include)
#  include_directories(${FHICL_INCLUDE_DIR})
#  
#  target_link_libraries(ReweightBaseLib ${ROOT_LIBRARIES})
#  #if(NOT DEFINED USE_FHICL OR NOT USE_FHICL)
#    include_directories(${CETLIB_EXCEPT_INCLUDE_DIR})
#    include_directories(${CETLIB_INCLUDE_DIR})
#    target_link_libraries(ReweightBaseLib ${FHICLCPP_LIBS}) 
#  #endif()
#  
#  install(TARGETS ReweightBaseLib DESTINATION lib)
#  
#  SET(MODULETargets ${MODULETargets};ReweightBaseLib PARENT_SCOPE)
#endif()
