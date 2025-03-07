#ifndef INDEX_H
#define INDEX_H
#include <iostream>
#include <vector>
#include "block.h"
#include <cstring>

// if isLeaf true, offset is direct offset
struct IndexEntry {
  unsigned offset; //Byte level offset FOR leaf nodes and BLOCK level offset for internal node 4bytes //
  float key; //FG_PCT_HOME value 4 bytes
  bool isLeaf; // 4
  IndexEntry() : offset(0), key(0), isLeaf(false) {};
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

  //TODO this should change the nearest last pointer to the end, not the actl end pointer
  void updateNodeBackPointer(unsigned offset){
    std::vector<Byte> temp;
    temp.resize(sizeof(unsigned));
    std::memcpy(temp.data(), &offset,  sizeof(offset));
    unsigned pos = numOfIndexEntries*sizeOfIndex;
    for(unsigned i = 0; i < sizeof(unsigned); ++i){
      block[i + pos] = temp[i];
    }
  }

  unsigned getNodeBackPointer(){
    std::vector<Byte> temp;
    temp.resize(sizeof(unsigned));
    unsigned pos = numOfIndexEntries*sizeOfIndex;
    for(unsigned i = 0; i < sizeof(unsigned); ++i){
      temp[i] = block[i+pos];
    }
    unsigned result{};
    std::memcpy(&result,temp.data(),sizeof(unsigned));
    return result;
  }


  ProxyIndex operator[](size_t index){
    if(index >= numOfIndexEntries){
      throw std::out_of_range("access exceeds block range");
    }
    return ProxyIndex(*this,index);
  }

};


#endif  // !INDEX_H