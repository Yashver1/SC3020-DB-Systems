#ifndef INDEXER_H
#define INDEXER_H

#include "bplustree.h"
#include "record.h"
#include "block.h"

class Indexer {
    bplustree tree;
    BlockView &block;

public:
    Indexer(BlockView &block);
    void buildIndex(const std::string &filename);
    Record searchByFGPctHome(float fgPctHome);
};

#endif
