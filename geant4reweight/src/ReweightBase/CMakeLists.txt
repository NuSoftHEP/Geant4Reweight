#file(GLOB sources *.cc)
#add_library(ReweightBaseLib
#  ${sources}
#)
#
#if(DEFINED USE_FHICL AND USE_FHICL)
#  add_dependencies(ReweightBaseLib fhiclcpp-simple)
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
#install(TARGETS ReweightBaseLib DESTINATION lib)
#
#target_link_libraries(ReweightBaseLib ${ROOT_LIBRARIES})
#if(NOT DEFINED USE_FHICL OR NOT USE_FHICL)
#  include_directories(${CETLIB_EXCEPT_INCLUDE_DIR})
#  include_directories(${CETLIB_INCLUDE_DIR})
#  target_link_libraries(ReweightBaseLib ${FHICLCPP_LIBS}) 
#endif()
#
#SET(MODULETargets ${MODULETargets};ReweightBaseLib PARENT_SCOPE)

art_make_library( LIBRARY_NAME ReweightBaseLib 
  SOURCE 
    G4ReweightStep.cc
    G4ReweightTraj.cc
    G4Reweighter.cc
    G4PiPlusReweighter.cc
    G4PiMinusReweighter.cc
    #G4ReweightTreeParser.cc
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
