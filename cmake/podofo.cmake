include(FetchContent)

set(PODOFO_GIT_TAG		0.10.2-rc3)
set(PODOFO_GIT_URL		https://github.com/pipiorg/podofo.git)
set(PODOFO_SOURCE_DIR	${CMAKE_SOURCE_DIR}/ext/podofo)
set(PODOFO_BINARY_DIR	${CMAKE_SOURCE_DIR}/ext-build/podofo)

set(PODOFO_BUILD_LIB_ONLY TRUE)

FetchContent_Declare(
	podofo
	GIT_REPOSITORY 	${PODOFO_GIT_URL}
	GIT_TAG			${PODOFO_GIT_TAG}
	SOURCE_DIR		${PODOFO_SOURCE_DIR}
	BINARY_DIR		${PODOFO_BINARY_DIR}
)

FetchContent_MakeAvailable(podofo)
