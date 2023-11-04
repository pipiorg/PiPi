include(FetchContent)

set(PODOFO_DEPS_GIT_URL		https://github.com/podofo/podofo-deps.git)
set(PODOFO_DEPS_SOURCE_DIR	${CMAKE_SOURCE_DIR}/ext/podofo-deps)

FetchContent_Declare(
	podofo-deps
	GIT_REPOSITORY 	${PODOFO_DEPS_GIT_URL}
	SOURCE_DIR		${PODOFO_DEPS_SOURCE_DIR}
)

file(GLOB PODOFO_DEPS_OPENSSL_FILES  "${PODOFO_DEPS_SOURCE_DIR}/3rdparty/openssl/lib/Win64/*")
file(COPY ${PODOFO_DEPS_OPENSSL_FILES} DESTINATION ${PODOFO_DEPS_SOURCE_DIR}/3rdparty/openssl)

set(ZLIB_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/zlib/lib/Win64/zlib.lib)
set(ZLIB_INCLUDE_DIR		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/zlib/include)
set(OPENSSL_ROOT_DIR		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/openssl)
set(JPEG_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libjpeg/lib/Win64/jpeg.lib)
set(JPEG_INCLUDE_DIR		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libjpeg/include)
set(TIFF_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libtiff/lib/Win64/tiff.lib)
set(TIFF_INCLUDE_DIR		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libtiff/include)
set(PNG_LIBRARY				${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libpng/lib/Win64/png.lib)
set(PNG_PNG_INCLUDE_DIR		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libpng/include)
set(LIBIDN_INCLUDE_DIR		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libidn/include)
set(LIBIDN_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libidn/lib/Win64/idn.lib)
set(FREETYPE_LIBRARY		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/freetype/lib/Win64/freetype.lib)
set(FREETYPE_INCLUDE_DIRS	${PODOFO_DEPS_SOURCE_DIR}/3rdparty/freetype/include/freetype2)
set(Fontconfig_LIBRARY		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/fontconfig/lib/Win64/fontconfig.lib)
set(Fontconfig_INCLUDE_DIR	${PODOFO_DEPS_SOURCE_DIR}/3rdparty/fontconfig/include)
set(LIBXML2_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libxml2/lib/Win64/libxml2.lib)
set(LIBXML2_INCLUDE_DIR		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libxml2/include)
