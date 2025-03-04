#include "diskmanager.h"

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

void DiskManager::txtToBinary(std::fstream &input, bool header) {
  std::fstream outputFile{
      "data.bin", outputFile.trunc | outputFile.out | outputFile.binary};
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
    //! impt step is after writing records zero out remaning space in blk and seek to
    //! start of nextblk to prepare for writing of next block
    std::vector<Byte> zeroedBuffer(remaining, 0);
    outputFile.write(reinterpret_cast<char *>(zeroedBuffer.data()), remaining);

    offset++;
  }

  outputFile.close();
}