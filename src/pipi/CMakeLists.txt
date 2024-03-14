include(podofo)
include(utfcpp)
include(spdlog)
include(icu)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/pipi PIPI_SOURCES)

add_library (pipi SHARED ${PIPI_SOURCES})
add_library(pipi::pipi ALIAS pipi)

set_property(TARGET pipi PROPERTY CXX_STANDARD 17)

target_link_libraries(pipi PUBLIC podofo::podofo)
target_link_libraries(pipi PUBLIC utf8cpp)
target_link_libraries(pipi PUBLIC spdlog)
target_link_libraries(pipi PUBLIC icu)

target_include_directories(pipi PUBLIC ${PODOFO_SOURCE_DIR}/src)
target_include_directories(pipi PUBLIC ${PODOFO_BINARY_DIR}/src/podofo)
target_include_directories(pipi PUBLIC ${UTFCPP_SOURCE_DIR}/source)
target_include_directories(pipi PUBLIC ${SPDLOG_SOURCE_DIR}/include)
