#pragma once

#include <rio/ccd_query.hpp>

#include <vector>
#include <string>

namespace rio {

std::vector<CCDQuery> read_rational_ccd_queries(const std::string& filename);

} // namespace rio
