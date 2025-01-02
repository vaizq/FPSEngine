//
// Created by vaige on 5.3.2024.
//

#include <catch2/catch_test_macros.hpp>
#include "engine/Util.h"
#include "engine/Perlin.h"
#include <iostream>


TEST_CASE( "filename extracting", "[utils]" )
{
    REQUIRE(Util::extractFilename("filename") == "filename");
    REQUIRE(Util::extractFilename("filename.txt") == "filename");
    REQUIRE(Util::extractFilename("/some/path/to/file/filename") == "filename");
    REQUIRE(Util::extractFilename("/some/../path/to/file/filename.txt") == "filename");
    REQUIRE(Util::extractFilename("filename.txt") == "filename");
    REQUIRE(Util::extractFilename("~/../.folder/filename.txt") == "filename");
}


TEST_CASE("Perlin2D basic test", "[perlin2D]")
{
    int w = 10;
    int h = 10;
    Perlin2D perlin(1, w, h);

    auto generateValues = [&]()
    {
        std::vector<float> values;
        for (int x = 0; x < w; ++x) {
            for (int y = 0; y < w; ++y) {
                values.push_back(perlin(glm::vec2(x + 0.5f, y + 0.5f)));
            }
        }
        return values;
    };

    auto values0 = generateValues();
    auto values1 = generateValues();

    REQUIRE(values0 == values1);
}
