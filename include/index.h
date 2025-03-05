#ifndef INDEX_H
#define INDEX_H
#include <iostream>

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
};

class IndexView {
  BlockView block;
  IndexEntry currentEntry;
};

#endif  // !INDEX_H