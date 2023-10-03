# Detects whether this is a top-level project
get_directory_property(HAS_PARENT PARENT_DIRECTORY)
if(HAS_PARENT)
  set(RIO_TOPLEVEL_PROJECT OFF)
else()
  set(RIO_TOPLEVEL_PROJECT ON)
endif()

# Check required CMake version
set(REQUIRED_CMAKE_VERSION "3.18.0")
if(RIO_TOPLEVEL_PROJECT)
  cmake_minimum_required(VERSION ${REQUIRED_CMAKE_VERSION})
else()
  # Don't use cmake_minimum_required here to avoid implicitly overriding parent policies
  if(${CMAKE_VERSION} VERSION_LESS ${REQUIRED_CMAKE_VERSION})
    message(FATAL_ERROR "CMake required version to build IPC Toolkit is ${REQUIRED_CMAKE_VERSION}")
  endif()
endif()

# Include user-provided default options if available. We do that before the main
# `project()` so that we can define the C/C++ compilers from the option file.
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/RationalCCDIOOptions.cmake)
  message(STATUS "Using local options file: ${CMAKE_CURRENT_SOURCE_DIR}/RationalCCDIOOptions.cmake")
  include(${CMAKE_CURRENT_SOURCE_DIR}/RationalCCDIOOptions.cmake)
endif()

# Enable ccache if available
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  option(RIO_WITH_CCACHE "Enable ccache when building RationalCCDIO" ${RIO_TOPLEVEL_PROJECT})
else()
  option(RIO_WITH_CCACHE "Enable ccache when building RationalCCDIO" OFF)
endif()
if(RIO_WITH_CCACHE AND CCACHE_PROGRAM)
  message(STATUS "Enabling Ccache support (${CCACHE_PROGRAM})")
  set(ccacheEnv
    CCACHE_BASEDIR=${CMAKE_BINARY_DIR}
    CCACHE_SLOPPINESS=clang_index_store,include_file_ctime,include_file_mtime,locale,pch_defines,time_macros
  )
  foreach(lang IN ITEMS C CXX)
    set(CMAKE_${lang}_COMPILER_LAUNCHER
      ${CMAKE_COMMAND} -E env ${ccacheEnv} ${CCACHE_PROGRAM}
    )
  endforeach()
endif()

################################################################################
# CMake Policies
################################################################################

cmake_policy(SET CMP0054 NEW) # Only interpret if() arguments as variables or keywords when unquoted.
cmake_policy(SET CMP0076 NEW) # target_sources() command converts relative paths to absolute.
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24")
  cmake_policy(SET CMP0135 NEW) # Set the timestamps of all extracted contents to the time of the extraction.
endif()

################################################################################

project(RationalCCDIO LANGUAGES CXX)

option(RIO_BUILD_TESTS             "Build unit-tests"            ${RIO_TOPLEVEL_PROJECT})
option(RIO_DOWNLOAD_SAMPLE_QUERIES "Download sample CCD queries"                     OFF)

# Set default minimum C++ standard
if(RIO_TOPLEVEL_PROJECT)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

### Configuration
set(RIO_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src/rio")
set(RIO_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/src")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/rio/")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/recipes/")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/find/")

# General CMake utils
include(rio_cpm_cache)
include(rio_use_colors)

# Generate position-independent code by default
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

################################################################################
# IPC Toolkit Library
################################################################################

# Add an empty library and fill in the list of sources in `src/CMakeLists.txt`.
add_library(rio_rio)
add_library(rio::rio ALIAS rio_rio)

# Fill in configuration options
# configure_file(
#   "${RIO_SOURCE_DIR}/config.hpp.in"
#   "${RIO_SOURCE_DIR}/config.hpp")

# Add source and header files to rio
add_subdirectory("${RIO_SOURCE_DIR}")

# Public include directory for IPC Toolkit
target_include_directories(rio_rio PUBLIC "${RIO_INCLUDE_DIR}")

################################################################################
# Optional Definitions
################################################################################

# For MSVC, do not use the min and max macros.
target_compile_definitions(rio_rio PUBLIC NOMINMAX)

################################################################################
# Dependencies
################################################################################

# GMP
include(gmp)
target_link_libraries(rio_rio PUBLIC GMP::GMP)

# JSON
include(json)
target_link_libraries(rio_rio PUBLIC nlohmann_json::nlohmann_json)

# Logger
include(spdlog)
target_link_libraries(rio_rio PUBLIC spdlog::spdlog)

if(RIO_DOWNLOAD_SAMPLE_QUERIES)
  include(sample_queries)
  target_link_libraries(rio_tests PUBLIC rio::sample_queries)
endif()

# Warnings
include(rio_warnings)
target_link_libraries(rio_rio PRIVATE rio::warnings)

################################################################################
# Compiler options
################################################################################

# Use C++17
target_compile_features(rio_rio PUBLIC cxx_std_17)

################################################################################
# Tests
################################################################################

# Enable unit testing at the root level
if(RIO_TOPLEVEL_PROJECT AND RIO_BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()