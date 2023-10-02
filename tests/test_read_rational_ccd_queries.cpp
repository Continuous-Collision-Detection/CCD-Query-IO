#include <catch2/catch_all.hpp>

#include <rio/read_rational_ccd_queries.hpp>

#include <fstream>

TEST_CASE("Read Rational CCD Query", "[read]")
{
    constexpr int N = 2;
    constexpr double r = 0.25;
    {
        std::ofstream of("test_read_rational_ccd_queries.csv");
        for (int i = 0; i < 8 * N; i++) {
            of << "1,4,1,4,1,4,1\n";
        }
    }

    std::vector<rio::CCDQuery> queries =
        rio::read_rational_ccd_queries("test_read_rational_ccd_queries.csv");

    REQUIRE(queries.size() == N);
    for (int q = 0; q < N; q++) {
        REQUIRE(queries[q].vertices.size() == 8);
        REQUIRE(queries[q].ground_truth == 1);
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 3; j++) {
                REQUIRE(queries[q].vertices[i][j] == r);
            }
        }
    }
}

TEST_CASE("Read Sample Queries", "[read][sample]")
{
    std::vector<rio::CCDQuery> queries = rio::read_rational_ccd_queries(
        std::string(RIO_SAMPLE_QUERIES_DIR)
        + "/unit-tests/vertex-face/data_0_0.csv");
    CHECK(queries.size() == 125);
}