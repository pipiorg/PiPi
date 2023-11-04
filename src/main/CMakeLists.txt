aux_source_directory(${MAIN_SOURCE_DIR} MAIN_SOURCES)

add_executable (main ${MAIN_SOURCES})

set_property(TARGET main PROPERTY CXX_STANDARD 17)

target_include_directories(main PRIVATE ${PIPI_SOURCE_DIR})

target_link_libraries(main PRIVATE pipi)