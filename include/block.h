#ifndef BLOCK_H
#define BLOCK_H
#include <fstream>
#include <iostream>
#include <vector>
#include "errors.h"
#include "utility.h"

extern const size_t RECORD_SIZE;

//TODO add field for totalNumofBlocks?? or maybe put in disk manager
class BlockView {
  std::vector<Byte> binaryData;

private:
  //! ProxyByte access for array operator overload so change on binaryData will
  //! save underlying fstream
  class ProxyByte {
  public:
    BlockView &parent;
    size_t index;
    ProxyByte(BlockView &p, size_t index) : parent(p), index(index){};
    operator Byte() const { return parent.binaryData[index]; }
    ProxyByte &operator=(Byte val) {
      parent.binaryData[index] = val;
      parent.saveAt(parent.currBlkOffset, parent.binaryData);
      return *this;
    }
  };

public:
  unsigned numOfRecords;
  unsigned blkSize;
  unsigned currBlkOffset;
  std::fstream &openFile;
  //! file should be in binary,in,out for non truncated random access writing
  //! and reading

  std::vector<Byte> data() { return this->binaryData; }
  void loadAt(unsigned blkOffset);
  void saveAt(unsigned blkOffset, std::vector<Byte> inputData);
  void next();
  BlockView(std::fstream &openFile, unsigned blkOffset);
  ProxyByte operator[](size_t index) {
    if (index >= binaryData.size()) {
      throw std::out_of_range("indexed access to block data out of range");
    }
    return ProxyByte(*this, index);
  }
};

#endif //  BLOCK_H