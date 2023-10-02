#include "read_rational_ccd_queries.hpp"

#include <rio/rational.hpp>
#include <rio/logger.hpp>

#include <array>
#include <fstream>
#include <sstream>

namespace rio {

std::vector<CCDQuery> read_rational_ccd_queries(const std::string& filename)
{
    // NOTE: If the file contains N lines, N % 8 == 0 because every 8 lines are
    // a single query.
    std::vector<CCDQuery> queries;

    // std::vector<std::array<double, 3>> vs;
    // vs.clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        logger().error("Unable to open file {}!", filename);
        throw std::runtime_error("Unable to open file!");
    }

    std::string line;
    for (int i = 0; file && std::getline(file, line); i++) {
        if (line[0] == '#')
            continue;

        if (i % 8 == 0) {
            // New query
            queries.emplace_back();
        }

        std::istringstream line_stream(line);
        // the first six are one vetex, the seventh is the result
        std::array<std::string, 7> line_items;
        for (std::string& item : line_items) {
            if (!std::getline(line_stream, item, ',')) {
                logger().error("Could not read file {} line {}!", filename, i);
                throw std::runtime_error("Could not read file!");
            }
        }

        for (int j = 0; j < 3; j++) {
            queries.back().vertices[i % 8][j] =
                Rational(line_items[2 * j + 0], line_items[2 * j + 1]);
        }

        if (i % 8 == 0) {
            queries.back().ground_truth = std::stoi(line_items[6]);
        } else if (
            bool(std::stoi(line_items[6])) != queries.back().ground_truth) {
            logger().error(
                "Ground truth mismatch in file {} line {}!", filename, i);
            throw std::runtime_error("Ground truth mismatch!");
        }
    }

    if (!file.eof()) {
        logger().error("Could not read file {}!", filename);
        throw std::runtime_error("Could not read file!");
    }

    return queries;
}

} // namespace rio
