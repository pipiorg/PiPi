include(podofo-deps)
include(podofo)

aux_source_directory(${PIPI_SOURCE_DIR} PIPI_SOURCES)

add_library (pipi SHARED ${PIPI_SOURCES})
add_library(pipi::pipi ALIAS pipi)

set_property(TARGET pipi PROPERTY CXX_STANDARD 17)

target_include_directories(pipi PRIVATE ${PODOFO_SOURCE_DIR}/src)
target_include_directories(pipi PRIVATE ${PODOFO_BINARY_DIR}/src/podofo)

target_link_libraries(pipi PRIVATE podofo::podofo)
