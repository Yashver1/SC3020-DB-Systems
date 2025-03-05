#include "indexmanager.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <cmath>
#include "record.h"

void IndexManager::createBPlusTree(std::fstream &dataFile, std::string name) {
    std::vector<std::pair<float, unsigned>> entries;
    
    dataFile.seekg(0, std::ios::beg);
    
    unsigned recordsPerBlock = BLOCK_SIZE / RECORD_SIZE;
    
    Record record;
    unsigned byteOffset = 0;
    while (dataFile.read(reinterpret_cast<char*>(&record), sizeof(Record))) {
        entries.push_back({record.FG_PCT_HOME, byteOffset});
        byteOffset += sizeof(Record);
    }
    
    std::sort(entries.begin(), entries.end());
    
    std::fstream indexFile(name, std::ios::binary | std::ios::in | 
                                std::ios::out | std::ios::trunc);
    
    if (!indexFile) {
        std::cerr << "Failed to create index file: " << name << std::endl;
        return;
    }
    
    unsigned entriesPerNode = (BLOCK_SIZE - sizeof(unsigned)) / sizeof(IndexEntry);
    
    std::vector<unsigned> parentLevelBlocks;
    unsigned nextBlockNum = 0;
    
    for (size_t i = 0; i < entries.size(); i += entriesPerNode) {
        std::vector<Byte> emptyBlock(BLOCK_SIZE, 0);
        indexFile.write(reinterpret_cast<char*>(emptyBlock.data()), BLOCK_SIZE);

        indexFile.seekg(nextBlockNum * BLOCK_SIZE);
        IndexView indexView(indexFile, nextBlockNum);
        
        size_t entriesInNode = std::min(entriesPerNode, entries.size() - i);
        
        for (size_t j = 0; j < entriesInNode; j++) {
            IndexEntry entry(entries[i + j].second, entries[i + j].first, true);
            indexView[j] = entry;
        }
        
        if (i + entriesPerNode < entries.size()) {
            indexView.updateNodeBackPointer(nextBlockNum + 1);
        } else {
            indexView.updateNodeBackPointer(0);
        }
        
        parentLevelBlocks.push_back(nextBlockNum);
        nextBlockNum++;
    }
    
    std::vector<unsigned> currentLevel = parentLevelBlocks;
    
    while (currentLevel.size() > 1) {
        std::vector<unsigned> nextLevel;
        
        for (size_t i = 0; i < currentLevel.size(); i += entriesPerNode) {
            std::vector<Byte> emptyBlock(BLOCK_SIZE, 0);
            indexFile.write(reinterpret_cast<char*>(emptyBlock.data()), BLOCK_SIZE);
            
            indexFile.seekg(nextBlockNum * BLOCK_SIZE);
            IndexView indexView(indexFile, nextBlockNum);
            
            size_t childrenCount = std::min(entriesPerNode, currentLevel.size() - i);
            
            for (size_t j = 0; j < childrenCount; j++) {
                IndexView childView(indexFile, currentLevel[i + j]);
                IndexEntry firstEntry = childView[0];
                
                IndexEntry entry(currentLevel[i + j], firstEntry.key, false);
                indexView[j] = entry;
            }
            
            if (i + entriesPerNode < currentLevel.size()) {
                indexView.updateNodeBackPointer(nextBlockNum + 1);
            } else {
                indexView.updateNodeBackPointer(0);
            }
            
            nextLevel.push_back(nextBlockNum);
            nextBlockNum++;
        }
        
        currentLevel = nextLevel;
    }
    
    std::fstream rootFile(name + ".meta", std::ios::binary | std::ios::out);
    if (rootFile) {
        unsigned rootBlockNum = currentLevel[0];
        rootFile.write(reinterpret_cast<char*>(&rootBlockNum), sizeof(unsigned));
        rootFile.close();
    }
    
    indexFile.close();
    
    this->blkMapCount[name] = nextBlockNum;
}