#include "indexmanager.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <cmath>
#include "record.h"
#include <utility>
#include <queue>
#include <functional>
void IndexManager::createBPlusTree(std::fstream &dataFile, unsigned totalNumBlocks,std::string name)  {
    std::priority_queue<std::pair<float, unsigned>,std::vector<std::pair<float,unsigned>>,std::greater<std::pair<float,unsigned>>> entries;
    std::fstream indexFile(name, indexFile.binary | indexFile.in | indexFile.out | indexFile.trunc);
    RecordView recordCursor{dataFile,0};

   //TODO REMOVE THE RECORDS THAT WERE ZERO INITLAISED CAUSE OF MISSING
    for(unsigned i = 0; i < totalNumBlocks; ++i){
        recordCursor.updateBlkOffset(i);
        for (unsigned j = 0; j < recordCursor.numOfRecords; ++j){
            Record curr = recordCursor[j];
            std::pair<float,unsigned> valAndPointer{curr.FG_PCT_HOME,curr.Header.offset};
            entries.push(valAndPointer);
        }
    }
    debug_print("Total num of entries : " << entries.size());
    unsigned maxN = (static_cast<unsigned int>(BLOCK_SIZE) - sizeof(unsigned))/sizeof(IndexEntry);
    unsigned minN = (maxN+1) / 2;
    unsigned maxNumberofLeafNodes = (entries.size() + 1)/minN;
    unsigned maxPointers = maxN+1;

    std::vector<std::vector<float>> currNodes;
    std::vector<unsigned> nodeNumber;

    //create leaf nodes
    for(unsigned i = 0; i < maxNumberofLeafNodes && !entries.empty(); ++i) {
        std::vector<Byte> initalData(BLOCK_SIZE,0);
        indexFile.write(reinterpret_cast<const char*>(initalData.data()),BLOCK_SIZE);
        IndexView indexCursor(indexFile,i);

        if(i > 0){
            indexCursor.updateBlockOffset(i-1);
            indexCursor.updateNodeBackPointer(i);
            indexCursor.updateBlockOffset(i);
        }

        nodeNumber.push_back(i);

        std::vector<float> curr;
        for(unsigned j = 0; j < maxN && !entries.empty(); ++j){
            auto currValAndPointer = entries.top();
            entries.pop();
            IndexEntry currIndexEntry{currValAndPointer.second,currValAndPointer.first,true};
            indexCursor[j] = currIndexEntry;
            curr.push_back(currValAndPointer.first);
        }

        currNodes.push_back(curr);
    }

    //create parent nodes

    std::vector<std::vector<float>> prevLevel = currNodes;
    std::vector<unsigned> prevNodeNumbers = nodeNumber;

    while(prevLevel.size() > 1){
        std::vector<std::vector<float>> currentLevel{};
        std::vector<unsigned> currentNodes{};

        std::vector<float> currInternalNode{};
        for(unsigned i = 0; i < prevNodeNumbers.size(); ++i){
            if(currInternalNode.size() >= maxPointers){
                //split node
                std::vector<float> newInternalNode{};
                currInternalNode.

            }
            
        }

    }





    // debugging print for node number 
    // std::ofstream logFile("bplustree_debug.log", std::ios::app);
    
    // logFile << "Debug - nodeNumber: ";
    // for (unsigned idx : nodeNumber) {
    //     logFile << idx << " ";
    // }
    // logFile << std::endl;

    // logFile << "Debug - currNodes contents:" << std::endl;
    // for (size_t i = 0; i < currNodes.size(); ++i) {
    //     logFile << "  Node " << i << " (ID: " << nodeNumber[i] << "): ";
    //     for (float val : currNodes[i]) {
    //         logFile << val << " ";
    //     }
    //     logFile << std::endl;
    // }
    
    // logFile.close();

    //create parent nodes
    indexFile.flush();
    indexFile.close();
}

