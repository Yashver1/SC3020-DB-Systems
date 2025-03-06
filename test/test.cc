#define CATCH_CONFIG_MAIN

#include <diskmanager.h>

#include <array>
#include <bitset>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "indexmanager.h"
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
  fstream logFile{"test_vector_vs_array_byte_parsing.txt", logFile.out | logFile.trunc};
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

  fstream log{"record_view_functions.txt", log.trunc | log.out | log.in};

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
  dm.linearScan(0.6, 0.9);
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
  std::fstream file{"temp_index.bin", file.binary | file.in |
                                          file.out | file.trunc};

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

TEST_CASE("IndexView updateNodeBackPointer functionality") {
  std::fstream file{"temp_index.bin", std::ios::binary | std::ios::in |
                                          std::ios::out | std::ios::trunc};

  std::vector<Byte> emptyBlock(BLOCK_SIZE, 0);
  file.write(reinterpret_cast<char *>(emptyBlock.data()), BLOCK_SIZE);
  file.seekg(0);

  IndexView indexView(file, 0);

  SECTION("updateNodeBackPointer writes back pointer correctly") {
    unsigned testOffset = 12345;

    indexView.updateNodeBackPointer(testOffset);

    unsigned pos = indexView.numOfIndexEntries * indexView.sizeOfIndex;

    std::vector<Byte> readBytes(sizeof(unsigned));
    for (unsigned i = 0; i < sizeof(unsigned); ++i) {
      readBytes[i] = indexView.block[pos + i];
    }

    unsigned readOffset = 0;
    std::memcpy(&readOffset, readBytes.data(), sizeof(unsigned));

    REQUIRE(readOffset == testOffset);
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
        IndexEntry entry{i * static_cast<unsigned>(BLOCK_SIZE),
                         static_cast<float>(i) / 10, (i % 2 == 0)};

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

TEST_CASE("DiskManager query and batchQuery") {
  std::fstream textFile{"games.txt", std::ios::in};
  DiskManager dm{};

  if (textFile.is_open()) {
    dm.txtToBinary(textFile, true);
    textFile.close();

    unsigned firstRecordOffset = 0;
    Record firstRecord = dm.query(firstRecordOffset, "data.bin");

    REQUIRE(firstRecord.Header.offset == 0);

    unsigned secondBlockOffset = BLOCK_SIZE;
    Record secondBlockRecord = dm.query(secondBlockOffset, "data.bin");

    REQUIRE(secondBlockRecord.Header.offset == secondBlockOffset);

    std::vector<unsigned> addresses = {0, static_cast<unsigned>(BLOCK_SIZE),
                                       static_cast<unsigned>(2 * BLOCK_SIZE)};
    std::vector<Record> records = dm.batchQuery(addresses, "data.bin");

    REQUIRE(records.size() == 3);

    REQUIRE(records[0].Header.offset == 0);
    REQUIRE(records[1].Header.offset == BLOCK_SIZE);
    REQUIRE(records[2].Header.offset == 2 * BLOCK_SIZE);

    unsigned recordsPerBlock = BLOCK_SIZE / RECORD_SIZE;
    unsigned midBlockOffset = BLOCK_SIZE + (recordsPerBlock / 2) * RECORD_SIZE;

    Record midBlockRecord = dm.query(midBlockOffset, "data.bin");
    REQUIRE(midBlockRecord.Header.offset == midBlockOffset);
  } else {
    WARN("Could not open games.txt, skipping test");
  }
}

TEST_CASE("DiskManager query and batchQuery with mid-block records") {
  std::fstream textFile{"games.txt", std::ios::in};
  DiskManager dm{};

  if (textFile.is_open()) {
    dm.txtToBinary(textFile, true);
    textFile.close();

    unsigned recordsPerBlock = BLOCK_SIZE / RECORD_SIZE;

    SECTION("Query records at various positions") {
      unsigned firstRecordOffset = 0;
      Record firstRecord = dm.query(firstRecordOffset, "data.bin");
      REQUIRE(firstRecord.Header.offset == firstRecordOffset);

      unsigned secondRecordOffset = RECORD_SIZE;
      Record secondRecord = dm.query(secondRecordOffset, "data.bin");
      REQUIRE(secondRecord.Header.offset == secondRecordOffset);
      if (recordsPerBlock >= 3) {
        unsigned midBlockOffset = (recordsPerBlock / 2) * RECORD_SIZE;
        Record midBlockRecord = dm.query(midBlockOffset, "data.bin");
        REQUIRE(midBlockRecord.Header.offset == midBlockOffset);
      }

      unsigned secondBlockOffset = BLOCK_SIZE;
      Record secondBlockRecord = dm.query(secondBlockOffset, "data.bin");
      REQUIRE(secondBlockRecord.Header.offset == secondBlockOffset);

      if (recordsPerBlock >= 3) {
        unsigned thirdInSecondOffset = BLOCK_SIZE + 2 * RECORD_SIZE;
        Record thirdInSecondRecord = dm.query(thirdInSecondOffset, "data.bin");
        REQUIRE(thirdInSecondRecord.Header.offset == thirdInSecondOffset);
      }
    }

    SECTION("Batch query with mixed positions") {
      std::vector<unsigned> addresses;

      addresses.push_back(0);

      if (recordsPerBlock >= 3) {
        addresses.push_back((recordsPerBlock / 2) * RECORD_SIZE);
      }

      addresses.push_back((recordsPerBlock - 1) * RECORD_SIZE);

      addresses.push_back(BLOCK_SIZE);

      if (recordsPerBlock >= 2) {
        addresses.push_back(BLOCK_SIZE + RECORD_SIZE);
      }

      std::vector<Record> records = dm.batchQuery(addresses, "data.bin");

      REQUIRE(records.size() == addresses.size());
      for (size_t i = 0; i < records.size(); i++) {
        REQUIRE(records[i].Header.offset == addresses[i]);
      }
    }
    std::remove("data.bin");
  } else {
    WARN("Could not open games.txt, skipping test");
  }
}


TEST_CASE("size check"){
  REQUIRE(sizeof(Record) == 28);
}

TEST_CASE("index manager"){
  std::vector<std::pair<float, unsigned>> entries;
  std::fstream textFile{"games.txt", std::ios::in};
  DiskManager dm{};
  dm.txtToBinary(textFile,true);
  unsigned totalNumBlocks = dm.blkMapCount["data.bin"];

  std::fstream log2File{"index_manager.txt", std::ios::out | std::ios::trunc};
  std::fstream dataFile{"data.bin", dataFile.in | dataFile.binary | dataFile.out };
  if(!dataFile.is_open()){
    debug_print("ERROR");
    throw std::logic_error("not open");
  }
  if(!log2File.is_open()){
    throw std::logic_error("log file not open");
  }
  RecordView recordCursor{dataFile,0};
  for(unsigned i = 0; i < totalNumBlocks; ++i){
      recordCursor.updateBlkOffset(i);
      for (unsigned j = 0; j < recordCursor.numOfRecords; ++j){
          Record curr = recordCursor[j];
          std::pair<float,unsigned> valAndPointer{curr.FG_PCT_HOME,curr.Header.offset};
          entries.push_back(valAndPointer);

      }
  }
  debug_print(entries.size());
  for(auto entry : entries){
    log2File << "FG_PCT_HOME: " << entry.first << " Reference: " << entry.second << '\n';
    log2File.flush();
  }

}


TEST_CASE("IndexManager Build B+ Tree Test") {
  std::fstream textFile{"games.txt", std::ios::in};
  DiskManager dm{};
  dm.txtToBinary(textFile, true);
  unsigned totalNumBlocks = dm.blkMapCount["data.bin"];

  debug_print("Total Number of Blocks: " << totalNumBlocks << "\n");
  textFile.close();
  
  std::string indexFileName = "test_index.bin";
  
  std::remove(indexFileName.c_str());
  
  std::fstream dataFile{"data.bin", std::ios::binary | std::ios::in | std::ios::out}; 
  REQUIRE(dataFile.is_open());
    
  IndexManager indexManager;
  indexManager.createBPlusTree(dataFile, totalNumBlocks, indexFileName);
  dataFile.close();
  
  {
    std::fstream indexFile{indexFileName, std::ios::binary | std::ios::in};
    REQUIRE(indexFile.is_open());
    
    std::fstream logFile{"index_manager_build_b+tree.txt", std::ios::out | std::ios::trunc};
    REQUIRE(logFile.is_open());
    
    indexFile.seekg(0, std::ios::end);
    std::streampos fileSize = indexFile.tellg();
    unsigned numIndexBlocks = fileSize / BLOCK_SIZE;
    indexFile.seekg(0, std::ios::beg);
    
    IndexView firstView(indexFile, 0);
    unsigned entriesPerBlock = firstView.numOfIndexEntries;
    
    logFile << "Index file created with " << numIndexBlocks << " blocks" << std::endl;
    logFile << "Each block can store " << entriesPerBlock << " index entries" << std::endl;
    logFile << "----------------------------------------------------" << std::endl;
    
    for (unsigned i = 0; i < numIndexBlocks; i++) {
      IndexView indexView(indexFile, i);
      logFile << "Node " << i << " contents: ";
      
      unsigned count = 0;
      while (count < indexView.numOfIndexEntries) {
        IndexEntry entry = indexView[count];
        if (entry.key == 0 && entry.offset == 0 && i > 1) break;
        
        logFile << entry.key << "→" << entry.offset;
        if (count < indexView.numOfIndexEntries - 1) logFile << ", ";
        count++;
      }

      logFile << "\nNode has " << count << " entries\n ";
      logFile << std::endl;
      
      unsigned backPointer = 0;
      unsigned pos = indexView.numOfIndexEntries * indexView.sizeOfIndex;
      std::vector<Byte> readBytes(sizeof(unsigned));
      for (unsigned j = 0; j < sizeof(unsigned); j++) {
        readBytes[j] = indexView.block[pos + j];
      }
      std::memcpy(&backPointer, readBytes.data(), sizeof(unsigned));
      
      if (backPointer != 0) {
        logFile << "  Node " << i << " has back pointer to node " << backPointer << std::endl;
      }
    }
    
    indexFile.close();
    logFile.close();
  }
  
  std::remove(indexFileName.c_str());
}


TEST_CASE("B+ Tree Bulk Loading verification test") {
  std::fstream textFile{"games.txt", std::ios::in};
  DiskManager dm{};
  dm.txtToBinary(textFile, true);
  unsigned totalNumBlocks = dm.blkMapCount["data.bin"];
  textFile.close();
  
  std::string indexFileName = "test_bplus_tree.bin";
  std::string logFileName = "B+_Tree_Bulk_Loading_verification_test.log";
  
  std::remove(indexFileName.c_str());
  
  std::fstream dataFile{"data.bin", std::ios::binary | std::ios::in | std::ios::out};
  REQUIRE(dataFile.is_open());
  
  IndexManager indexManager;
  indexManager.createBPlusTree(dataFile, totalNumBlocks, indexFileName);
  dataFile.close();
  
  std::fstream indexFile{indexFileName, std::ios::binary | std::ios::in | std::ios::out};
  std::fstream logFile{logFileName, std::ios::out | std::ios::trunc};
  REQUIRE(indexFile.is_open());
  REQUIRE(logFile.is_open());
  
  indexFile.seekg(0, std::ios::end);
  std::streampos fileSize = indexFile.tellg();
  unsigned numIndexBlocks = fileSize / BLOCK_SIZE;
  indexFile.seekg(0, std::ios::beg);
  
  SECTION("Verify index structure") {
    std::map<unsigned, std::vector<std::pair<float, unsigned>>> nodeContents;
    std::map<unsigned, std::vector<unsigned>> childPointers;
    
    for (unsigned i = 0; i < numIndexBlocks; i++) {
      IndexView indexView(indexFile, i);
      
      std::vector<std::pair<float, unsigned>> entries;
      unsigned j = 0;
      while (j < (BLOCK_SIZE - sizeof(unsigned)) / sizeof(IndexEntry)) {
        IndexEntry entry = indexView[j];
        if (entry.key == 0 && entry.offset == 0 && j > 0) break;
        entries.push_back({entry.key, entry.offset});
        j++;
      }
      
      nodeContents[i] = entries;
      
      unsigned backPtr = 0;
      std::vector<Byte> backPtrBytes(sizeof(unsigned));
      unsigned pos = (BLOCK_SIZE - sizeof(unsigned));
      for (unsigned k = 0; k < sizeof(unsigned); k++) {
        backPtrBytes[k] = indexView.block[pos + k];
      }
      std::memcpy(&backPtr, backPtrBytes.data(), sizeof(unsigned));
      
      std::vector<unsigned> pointers;
      for (const auto& entry : entries) {
        pointers.push_back(entry.second);
      }
      if (backPtr > 0) pointers.push_back(backPtr);
      childPointers[i] = pointers;
    }
    
    logFile << "\nB+ Tree Structure:\n";
    logFile << "Total nodes: " << numIndexBlocks << "\n";
    
    for (unsigned i = 0; i < numIndexBlocks; i++) {
      bool isLeaf = true;
      
      for (const auto& entry : nodeContents[i]) {
        if (entry.second < numIndexBlocks) {
          isLeaf = false;
          break;
        }
      }
      
      logFile << "Node " << i << " (" << (isLeaf ? "Leaf" : "Internal") << ") contains:\n";
      
      for (const auto& entry : nodeContents[i]) {
        logFile << "  Key: " << entry.first << ", Points to: " << entry.second << "\n";
      }
      
      if (!childPointers[i].empty()) {
        logFile << "  Back pointer: " << childPointers[i].back() << "\n";
      }
    }
    
    SECTION("Leaf node linkage") {
      std::vector<unsigned> leafNodes;
      for (unsigned i = 0; i < numIndexBlocks; i++) {
        if (!nodeContents[i].empty() && nodeContents[i][0].second >= numIndexBlocks) {
          leafNodes.push_back(i);
        }
      }
      
      for (size_t i = 0; i < leafNodes.size() - 1; i++) {
        IndexView node(indexFile, leafNodes[i]);
        unsigned backPtr = 0;
        std::vector<Byte> backPtrBytes(sizeof(unsigned));
        unsigned pos = (BLOCK_SIZE - sizeof(unsigned));
        for (unsigned k = 0; k < sizeof(unsigned); k++) {
          backPtrBytes[k] = node.block[pos + k];
        }
        std::memcpy(&backPtr, backPtrBytes.data(), sizeof(unsigned));
        
        REQUIRE(backPtr == leafNodes[i+1]);
      }
    }
    
    SECTION("Internal node separator keys") {
      std::vector<unsigned> internalNodes;
      for (unsigned i = 0; i < numIndexBlocks; i++) {
        if (!nodeContents[i].empty() && nodeContents[i][0].second < numIndexBlocks) {
          internalNodes.push_back(i);
        }
      }
      
      for (unsigned nodeId : internalNodes) {
        for (size_t i = 0; i < nodeContents[nodeId].size(); i++) {
          unsigned childId = childPointers[nodeId][i];
          if (childId < numIndexBlocks && !nodeContents[childId].empty()) {
            if (i < nodeContents[nodeId].size() - 1) {
              float separatorKey = nodeContents[nodeId][i+1].first;
              REQUIRE(nodeContents[childId][0].first <= separatorKey);
            }
          }
        }
      }
    }
  }
  
  logFile.close();
  indexFile.close();
  std::remove(indexFileName.c_str());
}



TEST_CASE("B+ Tree with Specific Keys test") {
  std::vector<float> keys  = {3.0f, 4.0f, 6.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 20.0f, 22.0f};

  std::priority_queue<std::pair<float, unsigned>,std::vector<std::pair<float,unsigned>>,std::greater<std::pair<float,unsigned>>> entries;

  for(auto key : keys){
    entries.push(std::pair<float,unsigned>{key,0});
  }

  std::fstream indexFile("test_btree.bin",indexFile.out | indexFile.in | indexFile.trunc | indexFile.binary);

  debug_print("Total num of entries : " << entries.size());
  unsigned maxN = 2;
  unsigned minN = (maxN+1) / 2;
  unsigned maxNumberofLeafNodes = (entries.size() + 1)/minN;
  unsigned maxPointers = maxN+1;

  std::vector<std::vector<float>> currNodes;
  std::vector<unsigned> nodeNumber;

  //create leaf nodes
  for(unsigned i = 0; i < maxNumberofLeafNodes && !entries.empty(); ++i) {
      std::vector<Byte> initalData(BLOCK_SIZE,0);
      indexFile.write(reinterpret_cast<const char*>(initalData.data()),BLOCK_SIZE);
      IndexView indexCursor(indexFile,i);

      if(i > 0){
          indexCursor.updateBlockOffset(i-1);
          indexCursor.updateNodeBackPointer(i);
          indexCursor.updateBlockOffset(i);
      }

      nodeNumber.push_back(i);

      std::vector<float> curr;
      for(unsigned j = 0; j < maxN && !entries.empty(); ++j){
          auto currValAndPointer = entries.top();
          entries.pop();
          IndexEntry currIndexEntry{currValAndPointer.second,currValAndPointer.first,true};
          indexCursor[j] = currIndexEntry;
          curr.push_back(currValAndPointer.first);
      }

      currNodes.push_back(curr);
  }

  //create parent nodes

  std::vector<std::vector<float>> prevLevel = currNodes;
  std::vector<unsigned> prevNodeNumbers = nodeNumber;

  // Debug prints
  debug_print("prevNodeNumbers: ");
  for(auto num : prevNodeNumbers) {
    std::cout << num << " ";
  }
  std::cout << std::endl;

  debug_print("prevLevel contents:");
  for(auto node : prevLevel) {
    std::cout << "Node values: ";
    for(auto val : node) {
      std::cout << val << " ";
    }
    std::cout << std::endl;
  }

  unsigned lastNodeNumber = prevNodeNumbers.back() + 1;

  while(prevLevel.size() > 1){
      std::vector<std::vector<float>> currentLevel{};
      std::vector<unsigned> currentNodes{};

      std::vector<float> currInternalNode{};

      for(unsigned i = 0; i < prevNodeNumbers.size(); ++i){
          if(currInternalNode.size() >= maxPointers){
              //split node
              unsigned splitBoundary = (maxPointers + 1)/2;
              std::vector<float> firstHalf(currInternalNode.begin(), currInternalNode.begin() + splitBoundary);
              std::vector<float> secondHalf(currInternalNode.begin() + splitBoundary, currInternalNode.end());
              currentLevel.push_back(firstHalf);
              currentNodes.push_back(lastNodeNumber);
              lastNodeNumber++;

              //become split node
              currInternalNode = secondHalf;
          }

          //push first key of each child node
          currInternalNode.push_back(prevLevel[i][0]);
      }

      //! first change
      if(!currInternalNode.empty()){
        currentLevel.push_back(currInternalNode);
        currentNodes.push_back(lastNodeNumber);
        lastNodeNumber++;
      }

      assert(prevNodeNumbers.size() == prevLevel.size() && "prevNodeNumbers not equal to prevLevel!");
      unsigned prevNodeidx{};
      for(unsigned i = 0; i < currentLevel.size(); ++i){
          std::vector<Byte> writeBuffer(BLOCK_SIZE,0);
          indexFile.write(reinterpret_cast<const char*>(writeBuffer.data()),BLOCK_SIZE);
          IndexView indexCursor(indexFile,currentNodes[i]);
          //skip first key value as its the smallest value subtree
          for(unsigned j = 1; j < currentLevel[i].size(); ++j){
              IndexEntry internalNodeIdxEntry{prevNodeNumbers[prevNodeidx],currentLevel[i][j]};
              indexCursor[j-1] = internalNodeIdxEntry;
              prevNodeidx++;
          }
          indexCursor.updateNodeBackPointer(prevNodeNumbers[prevNodeidx]);
          prevNodeidx++;
      }
      // Print out the current level and current nodes before assignment
      debug_print("Current Level contents:");
      for(auto& node : currentLevel) {
          std::cout << "Node values: ";
          for(auto val : node) {
          std::cout << val << " ";
          }
          std::cout << std::endl;
      }
      
      debug_print("Current Nodes: ");
      for(auto num : currentNodes) {
          std::cout << num << " ";
      }
      std::cout << std::endl;

      prevLevel = currentLevel;
      prevNodeNumbers = currentNodes;
  }

  // debugging print for node number 
  // std::ofstream logFile("bplustree_debug.txt", std::ios::app);
  
  // logFile << "Debug - nodeNumber: ";
  // for (unsigned idx : nodeNumber) {
  //     logFile << idx << " ";
  // }
  // logFile << std::endl;

  // logFile << "Debug - currNodes contents:" << std::endl;
  // for (size_t i = 0; i < currNodes.size(); ++i) {
  //     logFile << "  Node " << i << " (ID: " << nodeNumber[i] << "): ";
  //     for (float val : currNodes[i]) {
  //         logFile << val << " ";
  //     }
  //     logFile << std::endl;
  // }
  
  // logFile.close();

  //create parent nodes
  unsigned rootOffset = lastNodeNumber;

  indexFile.flush();
  indexFile.seekg(indexFile.beg);

  IndexView testCursor{indexFile,0};

  for(unsigned i = 0; i < 8; ++i){
    testCursor.updateBlockOffset(i);
    for(unsigned j = 0; j < 2; j++){
      IndexEntry curr = testCursor[j];
      debug_print("Curr entry: "<< curr);
    }
    debug_print("Last pointer: "<< testCursor.getNodeBackPointer());
  }
  
  indexFile.close();
} 


TEST_CASE("range_query vs bruteforce"){
  fstream inputFile{"games.txt", inputFile.out | inputFile.in };
  DiskManager dm{};
  dm.txtToBinary(inputFile,true);
  unsigned totalNumOfBlks = dm.blkMapCount["data.bin"];

  IndexManager im{};
  fstream dataFile{"data.bin", dataFile.in | dataFile.out | dataFile.binary};
  im.createBPlusTree(dataFile,totalNumOfBlks);


  auto addresses = im.rangeQuery(0.6,0.9);
  auto records = dm.batchQuery(addresses);

  fstream logFile{"range_vs_brute.txt", dataFile.out | dataFile.trunc | dataFile.in};
  for(auto record : records){
    logFile << record;
  }
}