#ifndef BLOCK_H
#define BLOCK_H
#include <fstream>
#include <iostream>
#include <vector>

#include "errors.h"
#include "utility.h"

class BlockView {
  std::vector<byte> data;
  unsigned currOffset;
  // just to track currentOffset for debugging purp
  unsigned numOfRecords;

 public:
  std::fstream &openFile;

  BlockView(std::fstream &openFile, unsigned offset)
      : data(BLOCK_SIZE),
        currOffset(offset),
        numOfRecords(0),
        openFile(openFile) {
    // use load method
    throw NotImplemented();
  }

  void loadAt(unsigned offset);
  void saveAt(unsigned offest);
  void next();
};

#endif  //  BLOCK_H