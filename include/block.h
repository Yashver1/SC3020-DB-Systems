#ifndef BLOCK_H
#define BLOCK_H
#include <fstream>
#include <iostream>
#include <vector>

#include "errors.h"
#include "utility.h"

extern const size_t RECORD_SIZE;

class BlockView {
  std::vector<Byte> binaryData;

 public:
  unsigned numOfRecords;
  unsigned blkSize;
  unsigned currBlkOffset;
  std::fstream &openFile;
  //! file should be in binary,in,out for non truncated random access writing
  //! and reading

  BlockView(std::fstream &openFile, unsigned blkOffset)
      : binaryData(BLOCK_SIZE),
        currBlkOffset(blkOffset),
        numOfRecords(BLOCK_SIZE / RECORD_SIZE),
        blkSize(BLOCK_SIZE),
        openFile(openFile) {
    loadAt(blkOffset);
  }

  std::vector<Byte> data() { return this->binaryData; }
  void loadAt(unsigned blkOffset);
  void saveAt(unsigned blkOffset, std::vector<Byte> inputData);
  void next();
};

#endif  //  BLOCK_H