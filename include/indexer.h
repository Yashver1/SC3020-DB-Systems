#ifndef INDEXER_H
#define INDEXER_H

#include "block.h"
#include "bplustree.h"
#include "record.h"

class Indexer {
  bplustree tree;
  BlockView &block;

 public:
  Indexer(BlockView &block);
  void buildIndex(const std::string &filename);
  Record searchByFGPctHome(float fgPctHome);
};

#endif
