# CMake build system for minisysex
# Copyright (C) 2020 Dale Whinham <daleyo@gmail.com>

cmake_minimum_required(VERSION 3.13)
project(minisysex ASM C)

# Look for Amiga-GCC
if(NOT AMIGA_GCC)
	if(DEFINED ENV{AMIGA_GCC})
		set(AMIGA_GCC $ENV{AMIGA_GCC} CACHE STRING "Path to Amiga-GCC.")
	else()
		message(FATAL_ERROR "Couldn't find Amiga-GCC. Please set the AMIGA_GCC environment variable.")
	endif()
endif()

# Look for required external tools
find_package(Git)

# If Git was found, add a custom target to generate version information for every build
if(GIT_FOUND)
	add_custom_target(version ALL
		COMMAND ${CMAKE_COMMAND} -DGIT_EXECUTABLE=${GIT_EXECUTABLE} -DAPP_NAME=${CMAKE_PROJECT_NAME} -P ${CMAKE_SOURCE_DIR}/cmake/version.cmake
		COMMENT "Generating version information using git"
		SOURCES src/version.c.in
	)

	# Prevent CMake from looking for this nonexistant file at configure time
	set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/version.c PROPERTIES GENERATED TRUE)
endif()

# Override release C flags (defaults to -O3)
set(CMAKE_C_FLAGS_RELEASE "-Os -s -fomit-frame-pointer")

# Executable target
add_executable(minisysex
src/minisysex.c
	# Sources
	src/log.c
	src/midicamd.c
	src/midiserial.c

	# Headers
	include/log.h
	include/midi.h
	include/midicamd.h
	include/midiserial.h
)

# If version information will be generated, add it to the sources list
if(GIT_FOUND)
	target_sources(minisysex PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/version.c)
endif()

set_target_properties(minisysex PROPERTIES C_STANDARD 99)

# Link options must have -mcrt=nix13, -msmall-code and -fbaserel to match compile options
target_compile_options(minisysex PRIVATE -mcrt=nix13 -msmall-code -fbaserel -mregparm -Wall -Werror -Wno-pointer-sign)
target_link_options(minisysex PRIVATE -mcrt=nix13 -msmall-code -fbaserel -mregparm)

target_compile_definitions(minisysex PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_link_libraries(minisysex amiga $<$<CONFIG:Debug>:debug>)
target_include_directories(minisysex PRIVATE include)

# Ensure version info is generated before building
if(GIT_FOUND)
	add_dependencies(minisysex version)
endif()
