include(FetchContent)

set(PODOFO_DEPS_GIT_URL		https://github.com/podofo/podofo-deps.git)
set(PODOFO_DEPS_SOURCE_DIR	${CMAKE_SOURCE_DIR}/ext/podofo-deps)

FetchContent_Declare(
	podofo-deps
	GIT_REPOSITORY 	${PODOFO_DEPS_GIT_URL}
	SOURCE_DIR		${PODOFO_DEPS_SOURCE_DIR}
)

FetchContent_MakeAvailable(podofo-deps)

if (WIN32)
	file(GLOB PODOFO_DEPS_OPENSSL_FILES  "${PODOFO_DEPS_SOURCE_DIR}/3rdparty/openssl/lib/Win64/*")
endif()

if (UNIX AND APPLE) 
	file(GLOB PODOFO_DEPS_OPENSSL_FILES  "${PODOFO_DEPS_SOURCE_DIR}/3rdparty/openssl/lib/macosx-x86_64/*")
endif()

if (UNIX AND NOT APPLE)
	file(GLOB PODOFO_DEPS_OPENSSL_FILES  "${PODOFO_DEPS_SOURCE_DIR}/3rdparty/openssl/lib/linux-x86_64/*")
endif()

file(COPY ${PODOFO_DEPS_OPENSSL_FILES} DESTINATION ${PODOFO_DEPS_SOURCE_DIR}/3rdparty/openssl)

set(ZLIB_INCLUDE_DIR		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/zlib/include)
set(JPEG_INCLUDE_DIR		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libjpeg/include)
set(TIFF_INCLUDE_DIR		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libtiff/include)
set(PNG_PNG_INCLUDE_DIR		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libpng/include)
set(LIBIDN_INCLUDE_DIR		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libidn/include)
set(FREETYPE_INCLUDE_DIRS	${PODOFO_DEPS_SOURCE_DIR}/3rdparty/freetype/include/freetype2)
set(Fontconfig_INCLUDE_DIR	${PODOFO_DEPS_SOURCE_DIR}/3rdparty/fontconfig/include)
set(LIBXML2_INCLUDE_DIR		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libxml2/include)

set(OPENSSL_ROOT_DIR		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/openssl)

if (WIN32)
	set(ZLIB_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/zlib/lib/Win64/zlib.lib)
	set(JPEG_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libjpeg/lib/Win64/jpeg.lib)
	set(TIFF_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libtiff/lib/Win64/tiff.lib)
	set(PNG_LIBRARY				${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libpng/lib/Win64/png.lib)
	set(LIBIDN_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libidn/lib/Win64/idn.lib)
	set(FREETYPE_LIBRARY		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/freetype/lib/Win64/freetype.lib)
	set(Fontconfig_LIBRARY		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/fontconfig/lib/Win64/fontconfig.lib)
	set(LIBXML2_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libxml2/lib/Win64/libxml2.lib)
endif()

if (UNIX AND APPLE)
	set(ZLIB_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/zlib/lib/macosx-x86_64/libzlib.a)
	set(JPEG_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libjpeg/lib/macosx-x86_64/libjpeg.a)
	set(TIFF_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libtiff/lib/macosx-x86_64/libtiff.a)
	set(PNG_LIBRARY				${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libpng/lib/macosx-x86_64/libpng.a)
	set(LIBIDN_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libidn/lib/macosx-x86_64/libidn.a)
	set(FREETYPE_LIBRARY		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/freetype/lib/macosx-x86_64/libfreetype.a)
	set(Fontconfig_LIBRARY		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/fontconfig/lib/macosx-x86_64/libfontconfig.a)
	set(LIBXML2_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libxml2/lib/macosx-x86_64/libxml2.a)
endif()

if (UNIX AND NOT APPLE)
	set(ZLIB_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/zlib/lib/linux-x86_64/libzlib.a)
	set(JPEG_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libjpeg/lib/linux-x86_64/libjpeg.a)
	set(TIFF_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libtiff/lib/linux-x86_64/libtiff.a)
	set(PNG_LIBRARY				${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libpng/lib/linux-x86_64/libpng.a)
	set(LIBIDN_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libidn/lib/linux-x86_64/libidn.a)
	set(FREETYPE_LIBRARY		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/freetype/lib/linux-x86_64/libfreetype.a)
	set(Fontconfig_LIBRARY		${PODOFO_DEPS_SOURCE_DIR}/3rdparty/fontconfig/lib/linux-x86_64/libfontconfig.a)
	set(LIBXML2_LIBRARY			${PODOFO_DEPS_SOURCE_DIR}/3rdparty/libxml2/lib/linux-x86_64/libxml2.a)
endif()