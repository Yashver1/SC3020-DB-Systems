#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "record.h"
#include "utility.h"

TEST_CASE("Block Size Test") { REQUIRE(BLOCK_SIZE != 0); }

TEST_CASE("Record Struct Test") {
  Record rec{};
  RecordHeader h{};
  REQUIRE(sizeof(h) == 16);
  REQUIRE(sizeof(rec) == 40);
}
