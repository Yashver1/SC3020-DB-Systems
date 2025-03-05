#define CATCH_CONFIG_MAIN

#include <diskmanager.h>

#include <array>
#include <bitset>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "block.h"
#include "catch2/catch.hpp"
#include "index.h"
#include "record.h"
#include "utility.h"

using namespace std;

TEST_CASE("Block Size Test") { REQUIRE(BLOCK_SIZE == 4096); }

TEST_CASE("Record Struct Test") {
  // !Assumption dates will be in 2000s but for memory saving we keep minus 2000
  // to fit in one byte
  string date = "24/12/2014";
  Record rec1{0, date, "1234", "100", "0.5", "0.75", "0.33", "10", "20", "1"};
  Record rec2{0, {24, 12, 14}, 1234, 100, 0.5, 0.75, 0.33, 10, 20, 1};

  REQUIRE(rec1.Header.offset == rec2.Header.offset);
  REQUIRE(rec1.TEAM_ID_HOME == rec2.TEAM_ID_HOME);
  REQUIRE(rec1.FG_PCT_HOME == rec2.FG_PCT_HOME);
  REQUIRE(rec1.FT_PCT_HOME == rec2.FT_PCT_HOME);
  REQUIRE(rec1.FG3_PCT_HOME == rec2.FG3_PCT_HOME);
  REQUIRE(rec1.GAME_DATE_EST[0] == rec2.GAME_DATE_EST[0]);
  REQUIRE(rec1.GAME_DATE_EST[1] == rec2.GAME_DATE_EST[1]);
  REQUIRE(rec1.GAME_DATE_EST[2] == rec2.GAME_DATE_EST[2]);

  REQUIRE(sizeof(Record) == 28);

  std::vector<Record> buffer{};
  buffer.push_back(rec1);
  buffer.push_back(rec2);

  REQUIRE(sizeof(buffer[0]) * buffer.size() == 28 * 2);
}

TEST_CASE("Utility Tests") {
  vector<string> splitDate{"24", "12", "14"};
  string date{"24/12/14"};
  REQUIRE(splitDate == split(date, "/"));

  string line{
      "22/12/2022	1610612740	126	0.484	0.926	0.382	"
      "25	46	1"};
  vector<string> splitLine{"22/12/2022", "1610612740", "126", "0.484", "0.926",
                           "0.382",      "25",         "46",  "1"};
  REQUIRE(splitLine == split(line, "\t"));
}

TEST_CASE("Disk Manager Tests") {
  fstream file{"games.txt", file.in};

  if (!file.is_open()) {
    std::cerr << "File not Open";
  }

  DiskManager disk{};
  disk.txtToBinary(file, true);
}

TEST_CASE("Memory Tests") {
  // Testing allocations as bytes is okay
  string date = "24/12/2014";
  Record rec1{0, date, "1234", "100", "0.5", "0.75", "0.33", "10", "20", "1"};
  std::array<Record, 1> buffer{};
  std::memcpy(&buffer, &rec1, sizeof(rec1));
  REQUIRE(sizeof(buffer) == RECORD_SIZE);
  Record rec2{};
  std::memcpy(&rec2, &buffer, sizeof(Record));
  REQUIRE(rec1 == rec2);
}

TEST_CASE("Test Vector vs Array byte parsing") {
  fstream textFile{"games.txt", textFile.in};
  DiskManager dm{};
  dm.txtToBinary(textFile, true);

  fstream inputFile{"data.bin", inputFile.binary | inputFile.in};
  fstream logFile{"log.txt", logFile.out | logFile.trunc};
  std::array<Record, 1> buffer{};
  std::vector<Byte> buffer2(RECORD_SIZE);

  inputFile.read(reinterpret_cast<char *>(&buffer), sizeof(Record));
  inputFile.seekg(-sizeof(Record), inputFile.cur);
  inputFile.read(reinterpret_cast<char *>(buffer2.data()), sizeof(Record));
  REQUIRE(memcmp(buffer.data(), buffer2.data(), sizeof(Record)) == 0);
}

TEST_CASE("Block View Functions") {
  fstream textFile{"games.txt", textFile.in};
  DiskManager dm{};
  dm.txtToBinary(textFile, true);

  Record emptyRecord{};

  fstream inputFile{"data.bin",
                    inputFile.binary | inputFile.in | inputFile.out};
  BlockView blockCursor{inputFile, 0};

  std::vector<Byte> thirdBytes(RECORD_SIZE, 1);
  for (int i = 0; i < RECORD_SIZE; ++i) {
    blockCursor[i] = thirdBytes[i];
  }

  std::vector<Byte> fourthBytes(RECORD_SIZE, 5);
  for (int i = 0; i < RECORD_SIZE; ++i) {
    fourthBytes[i] = blockCursor[i];
  }

  REQUIRE(thirdBytes == fourthBytes);
}

TEST_CASE("Record View Functions") {
  fstream textFile{"games.txt", textFile.in};
  DiskManager dm{};
  dm.txtToBinary(textFile, true);

  fstream inputFile{"data.bin",
                    inputFile.in | inputFile.out | inputFile.binary};

  fstream log{"log.txt", log.trunc | log.out | log.in};

  RecordView recordCursor{inputFile, 0};
  for (int i = 0; i < dm.blkMapCount["data.bin"]; i++) {
    recordCursor.updateBlkOffset(i);
    log << "Block " << i << '\n';
    for (int j = 0; j < recordCursor.numOfRecords; ++j) {
      Record curr = recordCursor[j];
      log << "Record" << j << curr << '\n';
    }
  }
}

TEST_CASE("Brute Force Linear Scan") {
  //! have to be in the same scope txtToBinary as that will fill up the total
  //! number of blocks
  fstream inputFile{"games.txt", inputFile.in};
  DiskManager dm{};
  dm.txtToBinary(inputFile, true);
  dm.linearScan(0.5, 0.59);
}

TEST_CASE("Index Struct") { REQUIRE(sizeof(IndexEntry) == 12); }

TEST_CASE("Block size verification") {
  fstream inputFile{"games.txt", inputFile.in};
  DiskManager dm{};
  dm.txtToBinary(inputFile, true);

  std::fstream file("data.bin", std::ios::binary | std::ios::in);
  file.seekg(0, std::ios::end);
  size_t fileSize = file.tellg();

  unsigned numBlocks = dm.blkMapCount["data.bin"];
  debug_print(numBlocks);
  REQUIRE(fileSize == numBlocks * BLOCK_SIZE);
}

TEST_CASE("IndexView basic functionality") {
  std::fstream file{"temp_index.bin", std::ios::binary | std::ios::in |
                                          std::ios::out | std::ios::trunc};

  // Initialize file with empty data
  std::vector<Byte> emptyBlock(BLOCK_SIZE, 0);
  file.write(reinterpret_cast<char *>(emptyBlock.data()), BLOCK_SIZE);
  file.seekg(0);

  IndexView indexView(file, 0);

  unsigned expectedEntries =
      (BLOCK_SIZE - sizeof(unsigned)) / sizeof(IndexEntry);

  SECTION("Constructor initializes properties correctly") {
    REQUIRE(indexView.sizeOfIndex == sizeof(IndexEntry));
    REQUIRE(indexView.numOfIndexEntries == expectedEntries);
  }

  SECTION("updateBlockOffset changes the block offset") {
    indexView.updateBlockOffset(1);
    REQUIRE(indexView.block.currBlkOffset == 1);
  }

  SECTION("IndexEntry operator works correctly") {
    IndexEntry testEntry{12, 0.5f, true};

    indexView[0] = testEntry;

    IndexEntry curr = indexView[0];
    debug_print(curr);

    IndexEntry retrievedEntry = indexView[0];
    REQUIRE(retrievedEntry.offset == testEntry.offset);
    REQUIRE(retrievedEntry.key == testEntry.key);
    REQUIRE(retrievedEntry.isLeaf == testEntry.isLeaf);

  }

  SECTION("Out of range access throws exception") {
    REQUIRE_THROWS_AS(indexView[indexView.numOfIndexEntries],
                      std::out_of_range);
  }

  SECTION("Multiple IndexEntries read/write works") {
    std::vector<IndexEntry> testEntries = {
        {100, 0.1f, true}, {200, 0.2f, false}, {300, 0.3f, true}};

    for (size_t i = 0; i < testEntries.size(); ++i) {
      indexView[i] = testEntries[i];
    }

    for (size_t i = 0; i < testEntries.size(); ++i) {
      IndexEntry retrievedEntry = indexView[i];
      REQUIRE(retrievedEntry.offset == testEntries[i].offset);
      REQUIRE(retrievedEntry.key == testEntries[i].key);
      REQUIRE(retrievedEntry.isLeaf == testEntries[i].isLeaf);
    }
  }

  file.close();
  std::remove("temp_index.bin");
}

TEST_CASE("IndexView with DiskManager integration") {
  std::fstream textFile{"games.txt", std::ios::in};
  DiskManager dm{};

  if (textFile.is_open()) {
    dm.txtToBinary(textFile, true);
    textFile.close();

    std::fstream indexFile{"index.bin",
                           std::ios::binary | std::ios::out | std::ios::trunc};

    std::vector<Byte> emptyBlock(BLOCK_SIZE, 0);
    indexFile.write(reinterpret_cast<char *>(emptyBlock.data()), BLOCK_SIZE);
    indexFile.close();

    std::fstream file{"index.bin",
                      std::ios::binary | std::ios::in | std::ios::out};

    IndexView indexView(file, 0);

    SECTION("Write and read IndexEntries") {
      for (unsigned i = 0; i < 5; ++i) {
        IndexEntry entry{i * BLOCK_SIZE, static_cast<float>(i) / 10,
                         (i % 2 == 0)};

        indexView[i] = entry;
      }

      for (unsigned i = 0; i < 5; ++i) {
        IndexEntry entry = indexView[i];
        REQUIRE(entry.offset == i * BLOCK_SIZE);
        REQUIRE(entry.key == Approx(static_cast<float>(i) / 10));
        REQUIRE(entry.isLeaf == (i % 2 == 0));
      }
    }

    file.close();
    std::remove("index.bin");
  } else {
    WARN("Could not open games.txt, skipping test");
  }
}
