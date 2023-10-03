################################################################################
# Tests
################################################################################

add_executable(rio_tests test_read_rational_ccd_queries.cpp)

################################################################################
# Required Libraries
################################################################################

target_link_libraries(rio_tests PUBLIC rio::rio)

include(catch2)
target_link_libraries(rio_tests PUBLIC Catch2::Catch2WithMain)

include(sample_queries)
target_link_libraries(rio_tests PUBLIC rio::sample_queries)

# Extra warnings (link last for highest priority)
include(rio_warnings)
target_link_libraries(rio_tests PRIVATE rio::warnings)

################################################################################
# Compiler options
################################################################################

# Test data directory definition
if(WIN32)
  target_compile_definitions(rio_tests PUBLIC RIO_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}\\\\data\\\\")
else()
  target_compile_definitions(rio_tests PUBLIC RIO_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")
endif()

################################################################################
# Register tests
################################################################################

FetchContent_GetProperties(catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

# Register tests
set(PARSE_CATCH_TESTS_ADD_TO_CONFIGURE_DEPENDS ON)
catch_discover_tests(rio_tests)
