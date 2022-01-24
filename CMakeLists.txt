cmake_minimum_required(VERSION 3.19)

project(charconv-float 
	VERSION 1.0.0
	DESCRIPTION "substitues for C++17 std::to_char and std::from_char for float and double in libstdc++ 10 and earlier"
	HOMEPAGE_URL "https://www.emocean.io"
	LANGUAGES CXX)

if(NOT CMAKE_SYSTEM_NAME STREQUAL Windows)

find_package(EmoceanBuildSystem)

add_library(
	${PROJECT_NAME} SHARED

	src/floating_to_chars.cc
)

target_compile_options(
	${PROJECT_NAME}

	PRIVATE
		-Wno-old-style-cast
		-Wno-return-type
		-Wno-unneeded-internal-declaration
		-Wno-unused-function
		-Wno-sometimes-uninitialized
)

target_include_directories(
	${PROJECT_NAME}
	
	PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/charconv-float>"
		"$<INSTALL_INTERFACE:include/charconv-float>"
)

if (EmoceanBuildSystem_FOUND)
	emocean_install_target(${PROJECT_NAME} NAMESPACE CharconvFloat)
	emocean_install_files(include/${PROJECT_NAME}/charconv DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/charconv-float" COMPONENT devel)
else()
	install(TARGETS ${PROJECT_NAME} NAMESPACE CharconvFloat RUNTIME ARCHIVE)
	install(FILES include/${PROJECT_NAME}/charconv DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/charconv-float")
endif()

endif()
