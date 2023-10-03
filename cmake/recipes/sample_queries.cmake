if(TARGET tight_inclusion::sample_queries)
  return()
endif()

include(ExternalProject)
include(FetchContent)

set(RIO_SAMPLE_QUERIES_DIR "${PROJECT_SOURCE_DIR}/tests/data/sample-queries/" CACHE PATH "Where should we download sample queries?")

ExternalProject_Add(
  sample_queries_download
  PREFIX "${FETCHCONTENT_BASE_DIR}/sample-queries"
  SOURCE_DIR ${RIO_SAMPLE_QUERIES_DIR}

  GIT_REPOSITORY https://github.com/Continuous-Collision-Detection/Sample-Queries.git
  GIT_TAG 4d6cce33477d8d5c666c31c8ea23e1aea97be371

  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON
)

# Create a dummy target for convenience
add_library(rio_sample_queries INTERFACE)
add_library(rio::sample_queries ALIAS rio_sample_queries)

add_dependencies(rio_sample_queries sample_queries_download)

target_compile_definitions(rio_sample_queries INTERFACE RIO_SAMPLE_QUERIES_DIR=\"${RIO_SAMPLE_QUERIES_DIR}\")