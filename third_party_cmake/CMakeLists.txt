# Copyright (c) third_party_cmake developers

cmake_minimum_required(VERSION 3.12)
project(third_party_cmake)

if((NOT DEFINED CMAKE_BUILD_TYPE) OR (NOT CMAKE_BUILD_TYPE) OR (CMAKE_BUILD_TYPE STREQUAL ""))
set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

set(DEPENDENCY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build-libraries)
file(MAKE_DIRECTORY ${DEPENDENCY_OUTPUT_DIRECTORY}/include)

include(ExternalProject)
ExternalProject_Add(dyncall-build
    URL https://dyncall.org/r1.4/dyncall-1.4.tar.gz
    URL_HASH SHA256=14437dbbef3b6dc92483f6507eaf825ab97964a89eecae8cb347a6bec9c32aae
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    PATCH_COMMAND sed -i -e "s/add_subdirectory(dyncallback)//" <SOURCE_DIR>/CMakeLists.txt
    CONFIGURE_COMMAND
		${CMAKE_COMMAND}
			-B <BINARY_DIR>
			-S <SOURCE_DIR>
			-DCMAKE_SYSTEM_NAME=Windows
			-DCMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}
			-DCMAKE_FIND_ROOT_PATH=/dev/null
			-DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER
			-DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY
			-DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY
			-DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY
			-DCMAKE_DISABLE_FIND_PACKAGE_PkgConfig=TRUE
			-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
			-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
			-DCMAKE_RC_COMPILER=${CMAKE_RC_COMPILER}
			-DCMAKE_INSTALL_PREFIX=${DEPENDENCY_OUTPUT_DIRECTORY}
			-DCMAKE_BUILD_TYPE=Release
			-DCMAKE_POLICY_VERSION_MINIMUM=3.12
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> && ${CMAKE_COMMAND} --build <BINARY_DIR> --target install
    BUILD_BYPRODUCTS ${DEPENDENCY_OUTPUT_DIRECTORY}/lib/libdyncall_s.a ${DEPENDENCY_OUTPUT_DIRECTORY}/lib/libdynload_s.a
    INSTALL_COMMAND ""
)
list(APPEND THIRDPARTY_DEPENDS dyncall-build)

foreach(CUR_DEPEND IN LISTS THIRDPARTY_DEPENDS)
ExternalProject_Get_Property(${CUR_DEPEND} BUILD_BYPRODUCTS)
list(APPEND THIRDPARTY_LIBRARIES ${BUILD_BYPRODUCTS})
endforeach()

add_custom_command(
    OUTPUT ${DEPENDENCY_OUTPUT_DIRECTORY}/lib/libthird_party_cmake.a
    COMMAND 
    	printf 'create %s\\n' ${DEPENDENCY_OUTPUT_DIRECTORY}/lib/libthird_party_cmake.a > ${DEPENDENCY_OUTPUT_DIRECTORY}/build.mri &&
    	printf 'addlib %s\\n' ${THIRDPARTY_LIBRARIES} >> ${DEPENDENCY_OUTPUT_DIRECTORY}/build.mri &&
    	printf 'save\\nend\\n' >> ${DEPENDENCY_OUTPUT_DIRECTORY}/build.mri &&
    	cat ${DEPENDENCY_OUTPUT_DIRECTORY}/build.mri | ${CMAKE_AR} -M
    DEPENDS ${THIRDPARTY_DEPENDS}
)

add_custom_command(
	OUTPUT ${DEPENDENCY_OUTPUT_DIRECTORY}/include/third_party_cmake.h
	COMMAND touch ${DEPENDENCY_OUTPUT_DIRECTORY}/include/third_party_cmake.h
	DEPENDS ${DEPENDENCY_OUTPUT_DIRECTORY}/lib/libthird_party_cmake.a
)

add_custom_target(main_output_target
	ALL
	DEPENDS ${DEPENDENCY_OUTPUT_DIRECTORY}/lib/libthird_party_cmake.a ${DEPENDENCY_OUTPUT_DIRECTORY}/include/third_party_cmake.h
)
