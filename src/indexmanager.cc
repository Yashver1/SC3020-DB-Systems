#include "indexmanager.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <cmath>
#include "record.h"
#include <utility>
void IndexManager::createBPlusTree(std::fstream &dataFile, unsigned totalNumBlocks,std::string name)  {
    std::vector<std::pair<float, unsigned>> entries;

    std::fstream indexFile(name, indexFile.binary | indexFile.in );
    RecordView recordCursor{dataFile,0};
    for(unsigned i = 0; i < totalNumBlocks; ++i){
        recordCursor.updateBlkOffset(i);
        for (unsigned j = 0; j < recordCursor.numOfRecords; ++j){
            Record curr = recordCursor[j];
            std::pair<float,unsigned> valAndPointer{curr.FG_PCT_HOME,curr.Header.offset};
            entries.push_back(valAndPointer);
        }
    }

    std::sort(entries.begin(),entries.end());
    




}