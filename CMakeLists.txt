cmake_minimum_required(VERSION 3.16)

project(dejenol
	DESCRIPTION "A library for working with the data files in Mordor: The Depths of Dejenol"
	LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# Project-wide settings
# ---------------------------------------------------------------------------

# Default to a Release build for single-config generators (Makefiles, Ninja).
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# dejenol static library
# ---------------------------------------------------------------------------

file(GLOB dejenol_SOURCES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/libDejenol/*.cpp
)
file(GLOB dejenol_HEADERS CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/libDejenol/*.h
)

add_library(dejenol STATIC
	${dejenol_SOURCES}
	${dejenol_HEADERS}
)

target_compile_features(dejenol PUBLIC cxx_std_17)

target_compile_options(dejenol INTERFACE
	$<$<CXX_COMPILER_ID:MSVC>:/W4>
	$<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wall;-Wextra;-Wpedantic>
)

target_include_directories(dejenol
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libDejenol>
)

# ---------------------------------------------------------------------------
# Examples
# ---------------------------------------------------------------------------

option(BUILD_EXAMPLES "Build the example programs" ON)

if(BUILD_EXAMPLES)

	add_executable(itemsheet examples/itemsheet.cpp)
	target_link_libraries(itemsheet	PUBLIC dejenol)

	add_executable(monstersheet examples/monstersheet.cpp)
	target_link_libraries(monstersheet PUBLIC dejenol)

	add_executable(pathfinder examples/pathfinder.cpp)
	target_link_libraries(pathfinder PUBLIC dejenol)

	add_executable(revealmap examples/revealmap.cpp)
	target_link_libraries(revealmap PUBLIC dejenol)

	add_executable(superseer
		examples/superseer.cpp
		examples/lodepng.cpp
	)
	target_link_libraries(superseer PUBLIC dejenol)

	add_executable(test examples/test.cpp)
	target_link_libraries(test PUBLIC dejenol)

endif()
