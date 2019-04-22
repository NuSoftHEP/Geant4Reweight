file(GLOB sources *.cc)
add_library(PredictionBaseLib
  ${sources}
)

include(${Geant4_USE_FILE})

message(${ROOT_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})

include_directories(${INCDIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS PredictionBaseLib DESTINATION lib)

target_link_libraries(PredictionBaseLib ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

SET(MODULETargets ${MODULETargets};PredictionBaseLib PARENT_SCOPE)
