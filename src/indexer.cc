#include "indexer.h"

#include <fstream>
#include <iostream>

Indexer::Indexer(BlockView &block)
    : tree(block), block(block) {}  // initialization

// 1: builds a B+ tree index on the FG PCT HOME
void Indexer::buildIndex(const std::string &filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Fail to open file: " << filename << "\n";
    return;
  }

  Record record;        // create an record instance
  unsigned offset = 0;  // position of each record
  while (file.read(reinterpret_cast<char *>(&record), sizeof(Record))) {
    float fgPctHome = record.FG_PCT_HOME;
    tree.insert(fgPctHome, offset);
    offset += sizeof(Record);
  }

  file.close();
}

// 2: searches the B+ tree based on the FG PCT HOME
Record Indexer::searchByFGPctHome(float fgPctHome) {
  unsigned offset;
  Record record;

  if (tree.search(fgPctHome, offset)) {
    block.openFile.seekg(offset);
    block.openFile.read(reinterpret_cast<char *>(&record), sizeof(Record));
    return record;
  } else {
    throw std::runtime_error("record not found");
  }
}