include(podofo-deps)
include(podofo)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/pipi PIPI_SOURCES)

add_library (pipi SHARED ${PIPI_SOURCES})
add_library(pipi::pipi ALIAS pipi)

set_property(TARGET pipi PROPERTY CXX_STANDARD 17)

target_link_libraries(pipi PRIVATE podofo::podofo)

target_include_directories(pipi PUBLIC ${PODOFO_SOURCE_DIR}/src)
target_include_directories(pipi PUBLIC ${PODOFO_BINARY_DIR}/src/podofo)