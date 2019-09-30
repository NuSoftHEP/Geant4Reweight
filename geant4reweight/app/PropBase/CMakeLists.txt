SET(PROP_APPS
  parMaker 
)

#if(DEFINED USE_FHICL AND USE_FHICL)
#  add_dependencies(PropBaseLib fhiclcpp-simple)
#else()
#  include_directories(${CETLIB_EXCEPT_INCLUDE_DIR})
#  include_directories(${CETLIB_INCLUDE_DIR})
#endif()
#
#include_directories(${ROOT_INCLUDE_DIRS})
#include_directories(${INCDIRS})
#
## Requried for fhiclcpp
##include_directories(${CMAKE_INSTALL_PREFIX}/include)
#include_directories(${FHICL_INCLUDE_DIR})
#
#foreach(appname ${PROP_APPS})
#  add_executable(${appname} ${appname}.cc)
#  target_link_libraries(${appname} ${MODULETargets})
#  target_link_libraries(${appname} ${ROOT_LIBRARIES})
#
#  if(NOT DEFINED USE_FHICL OR NOT USE_FHICL) 
#    target_link_libraries(${appname} ${FHICLCPP_LIBS}) 
#  endif()
#
#  install(TARGETS ${appname} DESTINATION bin)
#endforeach()

foreach(appname ${PROP_APPS})
  art_make_exec( NAME ${appname} 
    SOURCE 
      ${appname}.cc
    LIBRARIES
      ${ART_FRAMEWORK_CORE}
      ${ART_FRAMEWORK_PRINCIPAL}
      ${ART_FRAMEWORK_SERVICES_REGISTRY}
      ${ART_FRAMEWORK_SERVICES_BASIC}
      canvas
      ${MF_MESSAGELOGGER}
      ${FHICLCPP}
      cetlib cetlib_except
      ${ROOT_BASIC_LIB_LIST}
      PropBaseLib    
  )

endforeach()

