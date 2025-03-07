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
#include <cassert>
#include<unordered_set>
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


    std::vector<std::vector<float>> prevLevel = currNodes;
    std::vector<unsigned> prevNodeNumbers = nodeNumber;
    unsigned lastNodeNumber = prevNodeNumbers.back() + 1;

    while(prevLevel.size() > 1){
        std::vector<std::vector<float>> currentLevel{};
        std::vector<unsigned> currentNodes{};

        std::vector<float> currInternalNode{};

        for(unsigned i = 0; i < prevNodeNumbers.size(); ++i){
            if(currInternalNode.size() >= maxPointers){
                unsigned splitBoundary = (maxPointers + 1)/2;
                std::vector<float> firstHalf(currInternalNode.begin(), currInternalNode.begin() + splitBoundary);
                std::vector<float> secondHalf(currInternalNode.begin() + splitBoundary + 1, currInternalNode.end());
                currentLevel.push_back(firstHalf);
                currentNodes.push_back(lastNodeNumber);
                lastNodeNumber++;

                currInternalNode = secondHalf;
            }

            currInternalNode.push_back(prevLevel[i][0]);
        }

        if(!currInternalNode.empty()){
            currentLevel.push_back(currInternalNode);
            currentNodes.push_back(lastNodeNumber);
            lastNodeNumber++;
          }

        assert(prevNodeNumbers.size() == prevLevel.size() && "prevNodeNumbers not equal to prevLevel!");
        unsigned prevNodeidx{};
        for(unsigned i = 0; i < currentLevel.size(); ++i){
            std::vector<Byte> writeBuffer(BLOCK_SIZE,0);
            indexFile.write(reinterpret_cast<const char*>(writeBuffer.data()),BLOCK_SIZE);
            IndexView indexCursor(indexFile,currentNodes[i]);
            for(unsigned j = 1; j < currentLevel[i].size(); ++j){
                IndexEntry internalNodeIdxEntry{prevNodeNumbers[prevNodeidx],currentLevel[i][j]};
                indexCursor[j-1] = internalNodeIdxEntry;
                prevNodeidx++;
            }
            indexCursor.updateNodeBackPointer(prevNodeNumbers[prevNodeidx]);
            prevNodeidx++;
        }


        prevLevel = currentLevel;
        prevNodeNumbers = currentNodes;
    }

    // debugging print for node number 
    // std::ofstream logFile("bplustree_debug.txt", std::ios::app);
    
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
    rootOffset = lastNodeNumber - 1;

    indexFile.flush();
    indexFile.close();
}

std::vector<unsigned> IndexManager::rangeQuery(float lowerbound, float upperbound, std::string name) {
    std::fstream indexFile{name, indexFile.out | indexFile.in | indexFile.binary};

    std::vector<unsigned> addresses{};
    
    NUMBER_OF_INDEX_BLOCKS_INDEX_ACCESS++;
    IndexView indexCursor(indexFile, rootOffset); 
    
    IndexEntry curr = indexCursor[0];
    while(!curr.isLeaf) {
        unsigned idx = 0;
        while(idx < indexCursor.numOfIndexEntries) {
            curr = indexCursor[idx];
            if(curr.key > lowerbound || curr.key == 0) {
                break;
            }
            idx++;
        }
        
        unsigned nextBlockOffset = 0;
        if(idx == 0) { 
            nextBlockOffset = curr.offset;
        } else if(idx >= indexCursor.numOfIndexEntries) {
            nextBlockOffset = indexCursor.getNodeBackPointer();
        } else {
            IndexEntry last = indexCursor[idx-1];
            nextBlockOffset = last.offset;
        }
        
        NUMBER_OF_INDEX_BLOCKS_INDEX_ACCESS++;
        
        indexCursor.updateBlockOffset(nextBlockOffset);
        curr = indexCursor[0];
    }
    
    bool done = false;
    unsigned leafIdx = 0;
    
    while(!done) {
        while(leafIdx < indexCursor.numOfIndexEntries) { 
            curr = indexCursor[leafIdx];

            //first condition if reach end
            if((curr.key == 0 && curr.offset == 0) || curr.key > upperbound) {
                done = true;
                break;
            }
            
            if(curr.key >= lowerbound && curr.key <= upperbound) {
                addresses.push_back(curr.offset);
            }
            
            leafIdx++;
        }
        
        if(!done) {
            unsigned nextNode = indexCursor.getNodeBackPointer();
            if(nextNode == 0) {
                break;
            }
            
            NUMBER_OF_INDEX_BLOCKS_INDEX_ACCESS++;
            
            indexCursor.updateBlockOffset(nextNode);
            leafIdx = 0;
        }
    }
    
    indexFile.close();
    return addresses;
}

//TODO ensure last node of leaf node is min of n+1/2