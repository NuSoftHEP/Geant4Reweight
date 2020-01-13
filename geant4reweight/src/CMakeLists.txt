#if(USEART)
  add_subdirectory(PredictionBase)
  add_subdirectory(PropBase)
  add_subdirectory(ReweightBase)
  add_subdirectory(FitterBase)
#
#else()
#  LIST(APPEND INCDIRS ${CMAKE_CURRENT_SOURCE_DIR}/util)
#  LIST(APPEND INCDIRS ${CMAKE_CURRENT_SOURCE_DIR}/PredictionBase)
#  LIST(APPEND INCDIRS ${CMAKE_CURRENT_SOURCE_DIR}/PropBase)
#  LIST(APPEND INCDIRS ${CMAKE_CURRENT_SOURCE_DIR}/ReweightBase)
#  LIST(APPEND INCDIRS ${CMAKE_CURRENT_SOURCE_DIR}/FitterBase)
#
#  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/PredictionBase)
#  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/PropBase)
#  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ReweightBase)
#  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/FitterBase)
#                   
#  SET(LOCAL_INCDIRS ${INCDIRS})
#  SET(INCDIRS ${LOCAL_INCDIRS} PARENT_SCOPE)
#  
#  SET(LOCAL_MODULETargets ${MODULETargets})
#  LIST(REVERSE LOCAL_MODULETargets)
#  SET(MODULETargets ${LOCAL_MODULETargets} PARENT_SCOPE)
#endif()



