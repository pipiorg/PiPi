include(FetchContent)

set(ICU_GIT_URL		https://github.com/viaduck/icu-cmake.git)
set(ICU_SOURCE_DIR	${CMAKE_SOURCE_DIR}/ext/icu)
set(ICU_BINARY_DIR	${CMAKE_SOURCE_DIR}/ext-build/icu)

set(SYSTEM_ICU OFF)
set(BUILD_ICU ON)
set(ICU_BUILD_VERSION "74_2")
set(ICU_STATIC ON)

FetchContent_Declare(
	icu
	GIT_REPOSITORY 	${ICU_GIT_URL}
	SOURCE_DIR		${ICU_SOURCE_DIR}
	BINARY_DIR		${ICU_BINARY_DIR}
	GIT_SHALLOW     TRUE
)

FetchContent_MakeAvailable(icu)
