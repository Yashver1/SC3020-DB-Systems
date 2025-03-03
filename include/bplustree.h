#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <vector>
#include <iostream>
#include "block.h"
#include "record.h"
#include "utility.h"
#include "errors.h"

const unsigned MAX_KEYS = 3; //max number of keys a node can be hold

struct bplustreeNode
{
    bool isLeaf; //true if the node is a leaf
    unsigned numKeys; //current count of keys
    std::vector<unsigned> keys; // stores and manages key values
    std::vector<unsigned> children; //stores offsets to child nodes
    unsigned nextLeaf; //store the offset of the next leaf node

    bplustreeNode(bool leaf = false): isLeaf(leaf), numKeys(0), nextLeaf(0) {
        keys.resize(MAX_KEYS +1); 
        children.resize(MAX_KEYS+2); //resizing
    }

    bool isFull() const{return numKeys == MAX_KEYS; } //check whether a node is full

};

class bplustree{
    BlockView &block;
    unsigned rootOffset;

public:
    bplustree(BlockView &block);
    void insert(float key, unsigned offset);
    bool search(float key, unsigned &offset);
    void splitNode(bplustreeNode &node, unsigned offset);
    void printTree(unsigned offset, int level = 0);

};

#endif


