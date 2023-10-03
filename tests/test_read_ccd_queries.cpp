#include <catch2/catch_all.hpp>

#include <ccd_io/read_ccd_queries.hpp>
#include <ccd_io/logger.hpp>

#include <fstream>

void check_fails_to_read(std::string filename)
{
    try {
        ccd_io::read_ccd_queries(filename);
        FAIL();
    } catch (...) {
        SUCCEED();
    }
}

TEST_CASE("Nonexistent input", "[read]")
{
    ccd_io::logger().set_level(spdlog::level::off);
    check_fails_to_read("nonexistent_file.csv");
}

TEST_CASE("Bad input", "[read]")
{
    ccd_io::logger().set_level(spdlog::level::off);

    std::function<std::string(int)> line;
    SECTION("Too few lines")
    {
        line = [](int i) { return "0,1,0,1,0,1,0"; };
    }
    SECTION("Too few items per line")
    {
        line = [](int i) { return "0,1,0,1,0"; };
    }
    SECTION("Ground truth mismatch")
    {
        line = [](int i) {
            return fmt::format("0,1,0,1,0,1,{}", i % 8 == 0 ? 1 : 0);
        };
    }
    SECTION("Not a number")
    {
        line = [](int i) { return "a,1,0,1,0,1,1"; };
    }
    SECTION("Not a number (1/0)")
    {
        line = [](int i) { return "1,0,1,0,1,0,1"; };
    }
    SECTION("Not a number (0/0)")
    {
        line = [](int i) { return "0,0,1,0,1,0,1"; };
    }

    {
        std::ofstream of("bad_input.csv");
        for (int i = 0; i < 7; i++) {
            of << line(i) << "\n";
        }
    }

    check_fails_to_read("bad_input.csv");
}

TEST_CASE("Read CCD Query", "[read]")
{
    constexpr int N = 2;
    constexpr double r = 0.25;
    {
        std::ofstream of("test_read_ccd_queries.csv");
        for (int i = 0; i < 8 * N; i++) {
            of << "1,4,1,4,1,4,1\n";
        }
    }

    std::vector<ccd_io::CCDQuery> queries =
        ccd_io::read_ccd_queries("test_read_ccd_queries.csv");

    REQUIRE(queries.size() == N);
    for (int q = 0; q < N; q++) {
        CHECK(queries[q].vertices.size() == 8);
        CHECK(queries[q].ground_truth == 1);
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 3; j++) {
                CHECK(queries[q].vertices[i][j] == r);
            }
        }
    }
}

TEST_CASE("Read Sample Queries", "[read][sample]")
{
    std::vector<ccd_io::CCDQuery> queries = ccd_io::read_ccd_queries(
        std::string(CCD_IO_SAMPLE_QUERIES_DIR)
        + "/unit-tests/vertex-face/data_0_0.csv");
    CHECK(queries.size() == 125);
}

TEST_CASE("Read Split Sample Queries", "[read][sample]")
{
    std::vector<ccd_io::CCDQuery> queries = ccd_io::read_ccd_queries(
        std::string(CCD_IO_TEST_DATA_DIR) + "/split-ground-truth/queries.csv",
        std::string(CCD_IO_TEST_DATA_DIR)
            + "/split-ground-truth/mma_bool.json");
    REQUIRE(queries.size() == 2);
    CHECK(queries[0].ground_truth == false);
    CHECK(queries[1].ground_truth == false);
}