cmake_minimum_required(VERSION 2.8)
project(armish-fireplace)

##########################################################################################
# BUILD SETUP
##########################################################################################
option(USE_STATIC "Build with static libraries" OFF)
option(BUILD_RELEASE "Build for release" OFF)

##########################################################################################
# SOURCES
##########################################################################################
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(core_sources "")
set(module_sources "")

list(APPEND core_sources
		actions.cpp
		buffer/buffer_item.cpp
		buffer/queue_buffer.cpp
		net/global_zcontext.cpp
		net/simulation/request.cpp
		net/simulation/response.cpp
		net/simulation/client.cpp
		net/middleware/server.cpp
		net/middleware/zmq_server.cpp
		module/iproc_unit.cpp
		module/imodule.cpp
		module/module_manager.cpp
		armish_fireplace.cpp
)

list(APPEND module_sources
		module/brazil/brazil.cpp
		module/brazil/proc_unit/trx_circuit.cpp
		module/brazil/proc_unit/lcc_and_fpga.cpp
)

list(APPEND module_sources
		module/trabea/trabea.cpp
		module/trabea/iswitch_proc_unit.cpp
		module/trabea/proc_unit/circulator_switch.cpp
)

# Makes header includes more concise
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/tpls/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/tpls/rapidjson/include/")

##########################################################################################
# CONFIGURE BUILD TYPE
##########################################################################################
message("------------------------------")
message("Build Configuration:")

# Enable all compiler warnings that make sense
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -pedantic")
else()
	message(WARNING "Can't set comprehensive warnings in compiler")
endif()

if(BUILD_RELEASE)
	# Release build strips assert statements and enables compiler optimizations...
	set(CMAKE_BUILD_TYPE Release)
	
	# ...but it doesn't strip the binary
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
			"${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
	else()
		message(WARNING "Can't set strip flag in compiler")
	endif()
	
	
	message("Building for release")
else()
	# Debug build keeps assert statements and disables any compiler optimizations
	set(CMAKE_BUILD_TYPE Debug)
	
	message("Building for debug")
endif()

if(USE_STATIC)
	message("Using static libaries")
	
	set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_STATIC_RUNTIME ON)
	find_package(Boost COMPONENTS system thread program_options REQUIRED)
	link_directories(${Boost_LIBRARY_DIR})
	
	find_library(ZMQ_LIB libzmq REQUIRED)
	message(WARNING "ZMQ is still dynamically linked")
else()
	message("Using shared libraries")
	
	set(Boost_USE_STATIC_LIBS OFF)
	set(Boost_USE_STATIC_RUNTIME OFF)
	find_package(Boost COMPONENTS system thread program_options REQUIRED)
	link_directories(${Boost_LIBRARY_DIR})
	
	find_library(ZMQ_LIB zmq REQUIRED)
endif()

message("------------------------------")

##########################################################################################
# BUILD
##########################################################################################
add_executable(armish-fireplace
		${core_sources}
		${module_sources}
)

target_link_libraries(armish-fireplace
		pthread
		${Boost_LIBRARIES}
		${ZMQ_LIB}
		${LIBUDEV_LIBRARIES}
)

# Set the C++ standard level to C++11
if(CMAKE_MAJOR_VERSION GREATER 2 AND CMAKE_MINOR_VERSION GREATER 0)
	target_compile_features(armish-fireplace PRIVATE cxx_range_for)
else()
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
			"${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
			"${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	else()
		message(FATAL_ERROR "Your CMake is old and you have a weird compiler")
	endif()
endif()

##########################################################################################
# INSTALL
##########################################################################################
install(TARGETS armish-fireplace
		DESTINATION bin
)

##########################################################################################
# UNINSTALL
##########################################################################################
add_custom_target(uninstall
		COMMAND ${CMAKE_COMMAND} -P ${CMAKE_MODULE_PATH}/uninstall.cmake
)
