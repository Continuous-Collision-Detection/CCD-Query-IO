#pragma once

#include <rio/ccd_query.hpp>

#include <vector>
#include <string>

namespace rio {

/// @brief Read a CSV file containing rational CCD queries.
/// @param filename The name of the file to read.
/// @return A vector of CCD queries.
std::vector<CCDQuery> read_rational_ccd_queries(const std::string& filename);

/// @brief Read a CSV file containing rational CCD queries.
/// @param vertices_filename The name of the file containing the vertices.
/// @param ground_truth_filename The name of the file containing the ground
/// @return A vector of CCD queries.
std::vector<CCDQuery> read_rational_ccd_queries(
    const std::string& vertices_filename,
    const std::string& ground_truth_filename);

} // namespace rio
