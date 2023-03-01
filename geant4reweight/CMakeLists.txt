add_subdirectory(PredictionBase)
add_subdirectory(PropBase)
add_subdirectory(FitterBase)
add_subdirectory(ReweightBase)
add_subdirectory(app)
add_subdirectory(scripts)

# Install a symbolic link src -> . to preserve backwards compatibility
# for external header includes with /src/ in the path.
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink . src WORKING_DIRECTORY \"\${CMAKE_INSTALL_PREFIX}/${${CETMODULES_CURRENT_PROJECT_NAME}_INCLUDE_DIR}/${CETMODULES_CURRENT_PROJECT_NAME}\")")
