# Copyright (C) embedded ocean GmbH, Munich, Germany.
#
# This file is part of the Charconv Float Library.  This library is free
# software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# Under Section 7 of GPL version 3, you are granted additional
# permissions described in the GCC Runtime Library Exception, version
# 3.1, as published by the Free Software Foundation.
#
# You should have received a copy of the GNU General Public License and
# a copy of the GCC Runtime Library Exception along with this program;
# see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
# <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.19)

project(charconv-float 
	VERSION 1.0.0
	DESCRIPTION "substitues for C++17 std::to_char and std::from_char for float and double in libstdc++ 10 and earlier"
	HOMEPAGE_URL "https://www.emocean.io"
	LANGUAGES CXX)

###########################################################################
# Options and settings

option(CHARCONV_FLOAT_STATIC "whether to build charconv-float as a static library" OFF)

set(CHARCONV_FLOAT_NAMESPACE "__charconv_float" CACHE STRING "the inline namespace used for to_char and from_char")
set(CHARCONV_FLOAT_VISIBILITY "default" CACHE STRING "the ELF visibility used for to_char and from_char functions")

###########################################################################
# Check system features

try_compile(CHARCONV_FLOAT_HAVE_USELOCALE
	${CMAKE_CURRENT_BINARY_DIR}/check_uselocale
	${CMAKE_CURRENT_SOURCE_DIR}/config/check_uselocale.cc

	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

try_compile(CHARCONV_FLOAT_HAVE_XLOCALE_H
	${CMAKE_CURRENT_BINARY_DIR}/check_xlocale
	${CMAKE_CURRENT_SOURCE_DIR}/config/check_xlocale.cc

	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

try_compile(CHARCONV_FLOAT_HAVE_COMPATIBLE_GLIBCXX
	${CMAKE_CURRENT_BINARY_DIR}/check_libstdcxx_version
	${CMAKE_CURRENT_SOURCE_DIR}/config/check_libstdcxx_version.cc

	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

try_compile(CHARCONV_FLOAT_HAVE_INCLUDE_NEXT
	${CMAKE_CURRENT_BINARY_DIR}/check_include_next
	${CMAKE_CURRENT_SOURCE_DIR}/config/check_include_next.cc

	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

if(CHARCONV_FLOAT_HAVE_INCLUDE_NEXT AND CHARCONV_FLOAT_HAVE_COMPATIBLE_GLIBCXX)
	set(CHARCONV_FLOAT_PATCH_STD_HEADERS TRUE)
endif()

###########################################################################
# generate the config include

if(CHARCONV_FLOAT_HAVE_USELOCALE)
	set(CHARCONV_FLOAT_HAVE_FROM_CHAR TRUE)
endif()

set(CHARCONV_FLOAT_CONFIG_HEADER_FILE "${CMAKE_CURRENT_BINARY_DIR}/include/charconv-float-config.h")
configure_file(config/charconv-float-config.h.in "${CHARCONV_FLOAT_CONFIG_HEADER_FILE}" @ONLY)

###########################################################################
# define the library

if(CHARCONV_FLOAT_STATIC)
	set(CHARCONV_FLOAT_TARGET_TYPE STATIC)
else()
	set(CHARCONV_FLOAT_TARGET_TYPE SHARED)
endif()

add_library(
	${PROJECT_NAME} ${CHARCONV_FLOAT_TARGET_TYPE}

	src/c++17/floating_to_chars.cc
	src/c++17/floating_from_chars.cc
)

target_compile_definitions(
	${PROJECT_NAME}

	PRIVATE
		$<$<BOOL:${CHARCONV_FLOAT_HAVE_USELOCALE}>:CHARCONV_FLOAT_HAVE_USELOCALE=1>
		$<$<BOOL:${CHARCONV_FLOAT_HAVE_XLOCALE_H}>:CHARCONV_FLOAT_HAVE_XLOCALE_H=1>
)

target_compile_options(
	${PROJECT_NAME}

	PRIVATE
		$<$<CXX_COMPILER_ID:GNU,Clang>:-fsized-deallocation>

		$<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-old-style-cast>
		$<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-return-type>
		$<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-unneeded-internal-declaration>
		$<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-unused-function>
		$<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-sometimes-uninitialized>
		$<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-string-plus-int>
		$<$<CXX_COMPILER_ID:GNU,Clang>:-Wno-unused-variable>
)

target_include_directories(
	${PROJECT_NAME}
	
	PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
		"$<BUILD_INTERFACE:$<$<BOOL:${CHARCONV_FLOAT_PATCH_STD_HEADERS}>:${CMAKE_CURRENT_BINARY_DIR}/include/std>>"

		"$<INSTALL_INTERFACE:include/charconv-float>"
		"$<INSTALL_INTERFACE:$<$<BOOL:${CHARCONV_FLOAT_PATCH_STD_HEADERS}>:\${_IMPORT_PREFIX}/include/charconv-float/std>>"
)

set_target_properties(
	${PROJECT_NAME}
	
	PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON

		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
)

###########################################################################
# export and install the project

# Skip this entire section if no installation is desired
if(NOT CMAKE_SKIP_INSTALL_RULES)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CHARCONV_FLOAT_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/charconv-float")
set(CHARCONV_FLOAT_MODULEDIR "${CMAKE_INSTALL_DATADIR}/cmake/CharconvFloat")

set(CHARCONV_FLOAT_MODULE_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/module/CharconvFloatConfig.cmake")
set(CHARCONV_FLOAT_MODULE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/module/CharconvFloatVersion.cmake")

configure_package_config_file(
	config/CharconvFloatConfig.cmake.in
	${CHARCONV_FLOAT_MODULE_CONFIG_FILE}

	INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/CharconvFloat"
)

write_basic_package_version_file(
	${CHARCONV_FLOAT_MODULE_VERSION_FILE}

	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-targets RUNTIME ARCHIVE)

install(FILES include/charconv-float.h DESTINATION "${CHARCONV_FLOAT_INCLUDEDIR}")
install(FILES "${CHARCONV_FLOAT_CONFIG_HEADER_FILE}" DESTINATION "${CHARCONV_FLOAT_INCLUDEDIR}")

install(FILES include/std/charconv DESTINATION "${CHARCONV_FLOAT_INCLUDEDIR}/std")

install(EXPORT ${PROJECT_NAME}-targets NAMESPACE CharconvFloat FILE CharconvFloatTargets.cmake DESTINATION "${CHARCONV_FLOAT_MODULEDIR}")
install(FILES "${CHARCONV_FLOAT_MODULE_CONFIG_FILE}" DESTINATION "${CHARCONV_FLOAT_MODULEDIR}")
install(FILES "${CHARCONV_FLOAT_MODULE_VERSION_FILE}" DESTINATION "${CHARCONV_FLOAT_MODULEDIR}")
install(FILES config/PatchLibstdcxx.cmake DESTINATION "${CHARCONV_FLOAT_MODULEDIR}")
install(FILES config/check_include_next.cc DESTINATION "${CHARCONV_FLOAT_MODULEDIR}")
install(FILES config/check_libstdcxx_version.cc DESTINATION "${CHARCONV_FLOAT_MODULEDIR}")

endif(NOT CMAKE_SKIP_INSTALL_RULES)
