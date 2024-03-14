include(FetchContent)

set(UTFCPP_GIT_TAG		v4.0.4)
set(UTFCPP_GIT_URL		https://github.com/nemtrif/utfcpp.git)
set(UTFCPP_SOURCE_DIR	${CMAKE_SOURCE_DIR}/ext/utfcpp)
set(UTFCPP_BINARY_DIR	${CMAKE_SOURCE_DIR}/ext-build/utfcpp)

set(UTF8_TESTS OFF CACHE BOOL "" FORCE)
set(UTF8_SAMPLES OFF CACHE BOOL "" FORCE)

set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(
	utfcpp
	GIT_REPOSITORY 			${UTFCPP_GIT_URL}
	GIT_TAG					${UTFCPP_GIT_TAG}
	SOURCE_DIR				${UTFCPP_SOURCE_DIR}
	BINARY_DIR				${UTFCPP_BINARY_DIR}
	GIT_SHALLOW     		TRUE
	DOWNLOAD_NO_PROGRESS    OFF
    LOG_DOWNLOAD            ON
)

FetchContent_MakeAvailable(utfcpp)
