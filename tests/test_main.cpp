//
// Created by vaige on 5.3.2024.
//

#include <catch2/catch_test_macros.hpp>
#include "../src/core/Util.h"

TEST_CASE( "filename extracting", "[utils]" ) {
    REQUIRE(Util::extractFilename("filename") == "filename");
    REQUIRE(Util::extractFilename("filename.txt") == "filename");
    REQUIRE(Util::extractFilename("/some/path/to/file/filename") == "filename");
    REQUIRE(Util::extractFilename("/some/../path/to/file/filename.txt") == "filename");
    REQUIRE(Util::extractFilename("filename.txt") == "filename");
    REQUIRE(Util::extractFilename("~/../.folder/filename.txt") == "filename");
}
