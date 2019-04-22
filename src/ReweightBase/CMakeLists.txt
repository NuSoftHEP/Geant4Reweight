file(GLOB sources *.cc)
add_library(ReweightBaseLib
  ${sources}
)

if(DEFINED USE_FHICL AND USE_FHICL)
  add_dependencies(ReweightBaseLib fhiclcpp-simple)
endif()

include_directories(${ROOT_INCLUDE_DIRS})

include_directories(${INCDIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Requried for fhiclcpp
include_directories(${CMAKE_INSTALL_PREFIX}/include)

install(TARGETS ReweightBaseLib DESTINATION lib)

target_link_libraries(ReweightBaseLib ${ROOT_LIBRARIES})

SET(MODULETargets ${MODULETargets};ReweightBaseLib PARENT_SCOPE)
