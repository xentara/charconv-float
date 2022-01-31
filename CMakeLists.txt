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

set(CHARCONV_FLOAT_TARGET_TYPE SHARED CACHE STRING "the cmake target type for the library to build")
set(CHARCONV_FLOAT_NAMESPACE "__charconv_float" CACHE STRING "the inline namespace used for to_char and from_char")
set(CHARCONV_FLOAT_VISIBILITY "default" CACHE STRING "the ELF visibility used for to_char and from_char functions")

set_property(CACHE CHARCONV_FLOAT_TARGET_TYPE PROPERTY STRINGS SHARED STATIC OBJECT)

###########################################################################
# Check system features

try_compile(HAVE_USELOCALE
	${CMAKE_CURRENT_BINARY_DIR}/check_uselocale
	${CMAKE_CURRENT_SOURCE_DIR}/config/check_uselocale.cc

	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

try_compile(HAVE_XLOCALE_H
	${CMAKE_CURRENT_BINARY_DIR}/check_xlocale
	${CMAKE_CURRENT_SOURCE_DIR}/config/check_xlocale.cc

	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

try_compile(HAVE_COMPATIBLE_GLIBCXX
	${CMAKE_CURRENT_BINARY_DIR}/check_libstdcxx_version
	${CMAKE_CURRENT_SOURCE_DIR}/config/check_libstdcxx_version.cc

	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

try_compile(HAVE_INCLUDE_NEXT
	${CMAKE_CURRENT_BINARY_DIR}/check_include_next
	${CMAKE_CURRENT_SOURCE_DIR}/config/check_include_next.cc

	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

if(HAVE_INCLUDE_NEXT AND HAVE_COMPATIBLE_GLIBCXX)
	set(PATCH_STD_HEADERS TRUE)
endif()

###########################################################################
# generate the config include

if(HAVE_USELOCALE)
	set(CHARCONV_FLOAT_HAVE_FROM_CHAR TRUE)
endif()

set(CONFIG_HEADER_FILE "${CMAKE_CURRENT_BINARY_DIR}/include/charconv-float-config.h")
configure_file(config/charconv-float-config.h.in "${CONFIG_HEADER_FILE}" @ONLY)

###########################################################################
# define the library

add_library(
	${PROJECT_NAME} ${CHARCONV_FLOAT_TARGET_TYPE}

	src/c++17/floating_to_chars.cc
	src/c++17/floating_from_chars.cc
)

target_compile_definitions(
	${PROJECT_NAME}

	PRIVATE
		$<$<BOOL:${HAVE_USELOCALE}>:CHARCONV_FLOAT_HAVE_USELOCALE=1>
		$<$<BOOL:${HAVE_XLOCALE_H}>:CHARCONV_FLOAT_HAVE_XLOCALE_H=1>
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
		"$<INSTALL_INTERFACE:include/charconv-float>"
)

if(PATCH_STD_HEADERS)

target_include_directories(
	${PROJECT_NAME}
	
	PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/std>"
)

endif(PATCH_STD_HEADERS)

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

set(INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/charconv-float")
set(INSTALL_MODULEDIR "${CMAKE_INSTALL_DATADIR}/cmake/CharconvFloat")

set(MODULE_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/module/CharconvFloatConfig.cmake")
set(MODULE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/module/CharconvFloatVersion.cmake")

configure_package_config_file(
	config/CharconvFloatConfig.cmake.in
	${MODULE_CONFIG_FILE}

	INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/CharconvFloat"
)

write_basic_package_version_file(
	${MODULE_VERSION_FILE}

	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-targets RUNTIME ARCHIVE)
install(EXPORT ${PROJECT_NAME}-targets NAMESPACE CharconvFloat:: FILE CharconvFloatTargets.cmake DESTINATION "${INSTALL_MODULEDIR}")

install(
	FILES
		include/charconv-float.h
		"${CONFIG_HEADER_FILE}"
	
	DESTINATION "${INSTALL_INCLUDEDIR}"
)

install(
	FILES
		include/std/charconv
		
	DESTINATION "${INSTALL_INCLUDEDIR}/std"
)

install(
	FILES
		"${MODULE_CONFIG_FILE}"
		"${MODULE_VERSION_FILE}"
		config/PatchLibstdcxx.cmake
		config/check_include_next.cc
		config/check_libstdcxx_version.cc
	
	DESTINATION "${INSTALL_MODULEDIR}"
)

endif(NOT CMAKE_SKIP_INSTALL_RULES)
