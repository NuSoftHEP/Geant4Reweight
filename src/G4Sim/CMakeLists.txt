file(GLOB sources *.cc)
add_library(G4SimLib
  ${sources}
)

if(DEFINED USE_FHICL AND USE_FHICL)
  add_dependencies(G4SimLib fhiclcpp-simple)
endif()

include(${Geant4_USE_FILE})

message(${ROOT_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})

include_directories(${INCDIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Requried for fhiclcpp
include_directories(${CMAKE_INSTALL_PREFIX}/include)

install(TARGETS G4SimLib DESTINATION lib)

target_link_libraries(G4SimLib ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

SET(MODULETargets ${MODULETargets};G4SimLib PARENT_SCOPE)
