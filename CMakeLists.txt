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

# Support special features of embedded ocean GmbH's Synos build system
find_package(EmoceanBuildSystem1 QUIET)

if(NOT CMAKE_SYSTEM_NAME STREQUAL Windows)

###########################################################################
# Check system features

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

try_compile(HAVE_USELOCALE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/config/check_uselocale.cc
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

try_compile(HAVE_XLOCALE_H ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/config/check_xlocale.cc
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

try_compile(HAVE_COMPATIBLE_GLIBCXX ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/config/check_libstdcxx_version.cc
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

try_compile(HAVE_INCLUDE_NEXT ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/config/check_include_next.cc
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

if(HAVE_INCLUDE_NEXT AND HAVE_COMPATIBLE_GLIBCXX)
	set(CHARCONV_FLOAT_PATCH_STD_HEADERS TRUE)
endif()

###########################################################################
# generate the config include

if(HAVE_USELOCALE)
	set(CHARCONV_FLOAT_HAVE_FROM_CHAR TRUE)
endif()

set(CONFIG_HEADER_FILE "${CMAKE_CURRENT_BINARY_DIR}/include/bits/charconv-float-config.h")
configure_file(config/charconv-float-config.h.in "${CONFIG_HEADER_FILE}" @ONLY)

###########################################################################
# define the library

add_library(
	${PROJECT_NAME} SHARED

	src/floating_to_chars.cc
	src/floating_from_chars.cc
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

if (NOT EmoceanBuildSystem_FOUND)

	include(GNUInstallDirs)
	include(CMakePackageConfigHelpers)

	set(MODULE_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/CharconvFloatConfig.cmake")
	set(MODULE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/CharconvFloatVersion.cmake")

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

	install(FILES include/charconv.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/charconv-float")
	install(FILES include/std/charconv DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/charconv-float/std")
	install(FILES "${CONFIG_HEADER_FILE}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/charconv-float/bits")

	install(EXPORT ${PROJECT_NAME}-targets NAMESPACE CharconvFloat FILE CharconvFloatTargets.cmake DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/CharconvFloat")
	install(FILES "${MODULE_CONFIG_FILE}" DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/CharconvFloat")
	install(FILES "${MODULE_VERSION_FILE}" DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/CharconvFloat")
	install(FILES config/PatchLibstdcxx.cmake DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/CharconvFloat")
	install(FILES config/check_include_next.cc DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/CharconvFloat")
	install(FILES config/check_libstdcxx_version.cc DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/CharconvFloat")

else(NOT EmoceanBuildSystem_FOUND)

	emocean_install_target(${PROJECT_NAME}
		NAMESPACE CharconvFloat

		CMAKE_MODULE_INCLUDES
			config/PatchLibstdcxx.cmake

		CMAKE_MODULE_HELPER_FILES
			config/check_include_next.cc
			config/check_libstdcxx_version.cc
	)

	emocean_install_files(include/charconv.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/charconv-float" COMPONENT devel)
	emocean_install_files(include/std/charconv DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/charconv-float/std" COMPONENT devel)
	emocean_install_files("${CONFIG_HEADER_FILE}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/charconv-float/bits" COMPONENT devel)

endif(NOT EmoceanBuildSystem_FOUND)

endif(NOT CMAKE_SYSTEM_NAME STREQUAL Windows)
