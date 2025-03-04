#include "block.h"

#include <vector>

#include "errors.h"

void BlockView::saveAt(unsigned blkOffset, std::vector<Byte> inputData) {
  // save block data to file at offest
  this->currOffset = blkOffset;
  this->binaryData = inputData;
  this->openFile.seekp(blkOffset * blkSize);
  this->openFile.write(reinterpret_cast<const char*>(inputData.data()),
                       blkSize);
}

void BlockView::loadAt(unsigned blkOffset) {
  // load block data from file at offset
  this->currOffset = blkOffset;
  this->openFile.seekg(blkOffset * blkSize);
  this->openFile.read(reinterpret_cast<char*>(binaryData.data()), blkSize);
}

void BlockView::next() {
  // traverse files at offset , update offset
  this->currOffset += 1;
  this->openFile.seekg((currOffset * blkSize));
  this->openFile.read(reinterpret_cast<char*>(binaryData.data()), blkSize);
}
