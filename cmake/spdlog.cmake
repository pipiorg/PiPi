include(FetchContent)

set(SPDLOG_GIT_TAG		v1.12.0)
set(SPDLOG_GIT_URL		https://github.com/gabime/spdlog.git)
set(SPDLOG_SOURCE_DIR	${CMAKE_SOURCE_DIR}/ext/spdlog)
set(SPDLOG_BINARY_DIR	${CMAKE_SOURCE_DIR}/ext-build/spdlog)

set(SPDLOG_BUILD_LIB_ONLY TRUE)

FetchContent_Declare(
	spdlog
	GIT_REPOSITORY 	${SPDLOG_GIT_URL}
	GIT_TAG			${SPDLOG_GIT_TAG}
	SOURCE_DIR		${SPDLOG_SOURCE_DIR}
	BINARY_DIR		${SPDLOG_BINARY_DIR}
)

FetchContent_MakeAvailable(spdlog)