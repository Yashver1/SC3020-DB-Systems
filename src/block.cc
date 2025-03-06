#include "block.h"
#include <vector>
#include "errors.h"

void BlockView::saveAt(unsigned blkOffset, std::vector<Byte> inputData) {
  // save block data to file at offest
  this->currBlkOffset = blkOffset;
  this->binaryData = inputData;
  this->openFile.seekp(blkOffset * blkSize);
  this->openFile.write(reinterpret_cast<const char*>(inputData.data()),
                       blkSize);
}


void BlockView::loadAt(unsigned blkOffset) {
  // load block data from file at offset
  this->currBlkOffset = blkOffset;
  this->openFile.seekg(blkOffset * blkSize);
  this->openFile.read(reinterpret_cast<char*>(binaryData.data()), blkSize);
}

void BlockView::next() {
  // traverse files at offset , update offset
  this->currBlkOffset += 1;
  this->openFile.seekg((currBlkOffset * blkSize));
  this->openFile.read(reinterpret_cast<char*>(binaryData.data()), blkSize);
}

BlockView::BlockView(std::fstream &openFile, unsigned blkOffset)
      : binaryData(BLOCK_SIZE), currBlkOffset(blkOffset),
        numOfRecords(BLOCK_SIZE / RECORD_SIZE), blkSize(BLOCK_SIZE),
        openFile(openFile) {
    this->loadAt(blkOffset);
}

BlockView::BlockView(std::fstream &openFile, std::vector<Byte> intialData)
      : binaryData(BLOCK_SIZE), currBlkOffset(0),
        numOfRecords(BLOCK_SIZE / RECORD_SIZE), blkSize(BLOCK_SIZE),
        openFile(openFile) {
    this->saveAt(0,intialData);
    this->loadAt(0);//!uneceessary load?
}



