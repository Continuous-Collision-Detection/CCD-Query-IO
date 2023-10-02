set(SOURCES
  ccd_query.hpp
  logger.cpp
  logger.hpp
  rational.hpp
  read_rational_ccd_queries.cpp
  read_rational_ccd_queries.hpp
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Source Files" FILES ${SOURCES})
target_sources(rio_rio PRIVATE ${SOURCES})