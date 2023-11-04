cmake_minimum_required (VERSION 3.11)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("PiPi")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(podofo-deps)
include(podofo)

add_executable (main "main.cpp" "main.h")

target_include_directories(main PRIVATE ${PODOFO_SOURCE_DIR}/src)
target_include_directories(main PRIVATE ${PODOFO_BINARY_DIR}/src/podofo)
target_link_libraries(main PRIVATE podofo::podofo)

set_property(TARGET main PROPERTY CXX_STANDARD 17)
