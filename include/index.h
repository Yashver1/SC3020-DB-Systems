#ifndef INDEX_H
#define INDEX_H
#include <iostream>
#include <vector>
#include "block.h"

// if isLeaf true, offset is direct offset
struct IndexEntry {
  unsigned offset;
  float key;
  bool isLeaf;
  IndexEntry(unsigned offset, float key, bool isLeaf = false)
      : offset(offset), key(key), isLeaf(isLeaf) {};
  inline friend std::ostream &operator<<(std::ostream &os,
                                         const IndexEntry &idx) {
    if (idx.isLeaf) {
      os << "LeafNode-" << " Key: " << idx.key << " PointingTo: " << idx.offset;
    } else {
      os << "InternalNode- " << " Key: " << idx.key
         << " PointingTo: " << idx.offset;
    }
    return os;
  }
  IndexEntry() : offset(0), key(0), isLeaf(false) {};
};

class IndexView {
  public:
  BlockView block;
  size_t sizeOfIndex;
  unsigned numOfIndexEntries;

  class ProxyIndex {
    public:
    IndexView &parent;
    size_t index;

    ProxyIndex(IndexView &parent, size_t index) : parent(parent), index(index) {};

    operator IndexEntry(){
      std::vector<Byte> temp;
      temp.resize(parent.sizeOfIndex);
      for (unsigned i = 0; i < parent.sizeOfIndex ; ++i){
        temp[i] = parent.block[i + index*parent.sizeOfIndex];
      }
      IndexEntry realIndex{};
      std::memcpy(&realIndex,temp.data(),parent.sizeOfIndex);
      return realIndex;
    }

    ProxyIndex &operator=(IndexEntry newIndex){
      std::vector<Byte> temp;
      temp.resize(parent.sizeOfIndex);
      std::memcpy(temp.data(),&newIndex,parent.sizeOfIndex);

      for( unsigned i = 0 ; i < parent.sizeOfIndex ; ++i){
        parent.block[i + index*parent.sizeOfIndex] = temp[i];
      }
      return *this;
    }

  };


  IndexView(std::fstream &inputFile, unsigned blkOffset)  : block(inputFile,blkOffset) , sizeOfIndex(sizeof(IndexEntry)), numOfIndexEntries((BLOCK_SIZE - sizeof(unsigned)) / sizeof(IndexEntry)) {
  };

  void updateBlockOffset(unsigned blkOffset){
    block.loadAt(blkOffset);
  }

  ProxyIndex operator[](size_t index){
    if(index >= numOfIndexEntries){
      throw std::out_of_range("access exceeds block range");
    }
    return ProxyIndex(*this,index);
  }

};


#endif  // !INDEX_H