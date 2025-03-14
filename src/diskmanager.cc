#include "diskmanager.h"
#include "indexmanager.h"
#include "index.h"
#include <array>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
#include "record.h"
#include "utility.h"
#include <unordered_set>

void DiskManager::txtToBinary(std::fstream &input, bool header,
                              std::string name) {
  unsigned blkCount{};
  std::fstream outputFile{
      name, outputFile.trunc | outputFile.out | outputFile.binary};
  std::queue<std::string> buffer{};
  std::string temp{};

  while (std::getline(input, temp)) {
    buffer.push(temp);
  }

  if (header) {
    std::string headers = buffer.front();
    buffer.pop();
  }

  unsigned numPerBlocks = BLOCK_SIZE / RECORD_SIZE;
  unsigned remaining = BLOCK_SIZE - numPerBlocks * RECORD_SIZE;
  unsigned offset{};
  while (buffer.size() > 0) {
    for (unsigned i = 0; i < numPerBlocks && buffer.size() > 0; ++i) {
      std::string currLine = buffer.front();
      buffer.pop();
      std::vector<std::string> currFields = split(currLine, "\t");
      unsigned diskOffset = offset * BLOCK_SIZE + i * RECORD_SIZE;

      if(currFields[0].size() == 0 && currFields[1].size() == 0 && currFields[2].size())

      for (std::string &field : currFields) {
        if (field.size() == 0) {
          field = '0';
        }
      }

      Record currRecord{
          diskOffset,    currFields[0], currFields[1], currFields[2],
          currFields[3], currFields[4], currFields[5], currFields[6],
          currFields[7], currFields[8],
      };

      std::array<Record, 1> recordBuffer;
      std::memcpy(&recordBuffer, &currRecord, sizeof(Record));
      outputFile.write(reinterpret_cast<char *>(&recordBuffer), sizeof(Record));
    }
    //! impt step is after writing records zero out remaning space in blk and
    //! seek to start of nextblk to prepare for writing of next block
    std::vector<Byte> zeroedBuffer(remaining, 0);
    outputFile.write(reinterpret_cast<char *>(zeroedBuffer.data()), remaining);
    blkCount++;
    offset++;
  }

  //! after writing all records, check if we need to pad the final block which
  //! we do in this case
  if (outputFile.tellp() % BLOCK_SIZE != 0) {
    size_t currentPos = outputFile.tellp();
    size_t paddingNeeded = BLOCK_SIZE - (currentPos % BLOCK_SIZE);

    std::vector<char> padding(paddingNeeded, 0);

    outputFile.write(padding.data(), paddingNeeded);
  }

  this->blkMapCount.insert({name, blkCount});
  outputFile.close();
};

std::vector<Record> DiskManager::linearScan(float lowerBound, float upperBound, std::string name) {
    std::fstream inputFile{name, inputFile.out | inputFile.in | inputFile.binary};
    unsigned numOfBlocks = this->blkMapCount[name];
    RecordView recordCursor{inputFile, 0};
    
    std::vector<Record> results{};
    
    for (unsigned i = 0; i < numOfBlocks; ++i) {
        recordCursor.updateBlkOffset(i);
        
        NUMBER_OF_DATA_BLOCKS_BRUTE_FORCE++;
        
        for (unsigned j = 0; j < recordCursor.numOfRecords; ++j) {
            Record curr = recordCursor[j];
            if (curr.FG_PCT_HOME >= lowerBound && curr.FG_PCT_HOME <= upperBound)
                results.push_back(curr);
        }
    }
    
    inputFile.close();
    return results;
};

Record DiskManager::query(unsigned address , std::string name ){


  unsigned blkIndex = address / BLOCK_SIZE;
  unsigned recIndex = (address % BLOCK_SIZE) / RECORD_SIZE;

  std::fstream dataFile{name, dataFile.in | dataFile.out};
  RecordView recordCursor(dataFile, 0);
  recordCursor.updateBlkOffset(blkIndex);

  Record found = recordCursor[recIndex];
  return found;
  dataFile.close();
  
};

std::vector<Record> DiskManager::batchQuery(std::vector<unsigned> &addresses , std::string name){
  std::vector<Record> results{};
  NUMBER_OF_DATA_BLOCKS_INDEX_ACCESS=0;
  std::unordered_set<unsigned> setOfBlks;

  std::fstream dataFile{name, dataFile.in | dataFile.out};
  RecordView recordCursor(dataFile,0);
  for(unsigned address : addresses){
    unsigned blkIndex = address / BLOCK_SIZE;
    unsigned recIndex = (address % BLOCK_SIZE) / RECORD_SIZE;
    if(setOfBlks.find(blkIndex) == setOfBlks.end()){
      setOfBlks.insert(blkIndex);
    } else {
      NUMBER_OF_DATA_BLOCKS_INDEX_ACCESS++;
    }


    recordCursor.updateBlkOffset(blkIndex);
    Record found = recordCursor[recIndex];
    results.push_back(found);
  }

  dataFile.close();
  return results;
};