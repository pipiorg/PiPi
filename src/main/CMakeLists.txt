aux_source_directory(${PROJECT_SOURCE_DIR}/src/main MAIN_SOURCES)

add_executable (main ${MAIN_SOURCES})

set_property(TARGET main PROPERTY CXX_STANDARD 17)

target_link_libraries(main PRIVATE pipi::pipi)

target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/src/pipi)
