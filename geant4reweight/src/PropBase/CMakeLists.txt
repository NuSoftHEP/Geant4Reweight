#file(GLOB sources *.cc)
#add_library(PropBaseLib
#  ${sources}
#)
#
#if(DEFINED USE_FHICL AND USE_FHICL)
#  add_dependencies(PropBaseLib fhiclcpp-simple)
#endif()
#
#include_directories(${ROOT_INCLUDE_DIRS})
#
#include_directories(${INCDIRS})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#
## Requried for fhiclcpp
##include_directories(${CMAKE_INSTALL_PREFIX}/include)
#include_directories(${FHICL_INCLUDE_DIR})
#
#install(TARGETS PropBaseLib DESTINATION lib)
#
#target_link_libraries(PropBaseLib ${ROOT_LIBRARIES})
#if(NOT DEFINED USE_FHICL OR NOT USE_FHICL)
#  include_directories(${CETLIB_EXCEPT_INCLUDE_DIR})
#  include_directories(${CETLIB_INCLUDE_DIR})
#  target_link_libraries(PropBaseLib ${FHICLCPP_LIBS}) 
#endif()
#
#SET(MODULETargets ${MODULETargets};PropBaseLib PARENT_SCOPE)

art_make_library( LIBRARY_NAME PropBaseLib 
  SOURCE 
    G4ReweightParameterMaker.cc
    G4ReweightThrowManager.cc
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
)
install_headers()
install_source()
