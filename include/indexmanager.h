#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H
#include "index.h"
#include <unordered_map>
#include <string>

class IndexManager{
    std::unordered_map<std::string,unsigned> blkMapCount;
    IndexManager() : blkMapCount{}{};
    void createBPlusTree(std::fstream &dataFile, std::string name = "index.bin");
    std::vector<unsigned> rangeQuery(){throw NotImplemented();};
    unsigned singleQuery(){throw NotImplemented();};

};



#endif
