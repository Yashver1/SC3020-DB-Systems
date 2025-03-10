#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H
#include "index.h"
#include <unordered_map>
#include <string>

class IndexManager{
    public:
    std::unordered_map<std::string,unsigned> blkMapCount;
    unsigned rootOffset;
    IndexManager() : blkMapCount{}, rootOffset{} {};
    void createBPlusTree(std::fstream &dataFile, unsigned totalNumBlocks, std::string name = "index.bin" );
    std::vector<unsigned> rangeQuery(float lowerbound, float upperbound, std::string name = "index.bin");
    unsigned singleQuery(){throw NotImplemented();};
};

//range of FGT_PCT_HOME VALUES or single value -> offset(s)

#endif
