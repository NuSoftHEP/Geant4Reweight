#SET(SRC_FILES)
#LIST(APPEND SRC_FILES
#     )


file(GLOB sources *.cc)
add_library(G4SimLib
  ${sources}
)

include(${Geant4_USE_FILE})

message(${ROOT_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})

include_directories(${INCDIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS G4SimLib DESTINATION lib)

target_link_libraries(G4SimLib ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

SET(MODULETargets ${MODULETargets};G4SimLib PARENT_SCOPE)
