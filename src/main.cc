#include <record.h>
#include <diskmanager.h>
#include <indexmanager.h>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <cmath>
#include <unordered_set>
#include <numeric>
#include "utility"

void Task1(std::ofstream &answer, DiskManager &dm) {
    answer << "============== TASK 1: STORAGE COMPONENT ==============\n\n";
    answer << "Record Structure:\n";
    answer << "- Header (offset): 4 bytes\n";
    answer << "- TEAM_ID_HOME: 4 bytes\n";
    answer << "- FG_PCT_HOME: 4 bytes (float)\n";
    answer << "- FT_PCT_HOME: 4 bytes (float)\n";
    answer << "- FG3_PCT_HOME: 4 bytes (float)\n";
    answer << "- GAME_DATE_EST: 3 bytes (day, month, year-2000)\n";
    answer << "- AST_HOME: 1 byte\n";
    answer << "- REB_HOME: 1 byte\n";
    answer << "- PTS_HOME: 1 byte\n";
    answer << "- HOME_TEAM_WINS: 1 byte\n";
    answer << "- Padding: 1 byte\n\n";
    
    unsigned recordSize = sizeof(Record);
    NUMBER_OF_RECORDS_PER_BLOCK = BLOCK_SIZE / recordSize;
    NUMBER_OF_BLOCKS = dm.blkMapCount["data.bin"];
    
    std::fstream dataFile{"data.bin", std::ios::binary | std::ios::in};
    RecordView recordView(dataFile, 0);
    
    NUMBER_OF_RECORDS = 0;
    for (unsigned i = 0; i < NUMBER_OF_BLOCKS; i++) {
        recordView.updateBlkOffset(i);
        for (unsigned j = 0; j < NUMBER_OF_RECORDS_PER_BLOCK; j++) {
            Record record = recordView[j];
            if (record.TEAM_ID_HOME != 0) {
                NUMBER_OF_RECORDS++;
            }
        }
    }
    dataFile.close();
    
    answer << "Statistics:\n";
    answer << "- Record Size: " << recordSize << " bytes\n";
    answer << "- Total Number of Records: " << NUMBER_OF_RECORDS << "\n";
    answer << "- Usual Number of Records per Block: " << NUMBER_OF_RECORDS_PER_BLOCK << "\n";
    answer << "- Number of Blocks: " << NUMBER_OF_BLOCKS << "\n\n";
}

void Task2(std::ofstream &answer, IndexManager &im) {
    answer << "============== TASK 2: INDEXING COMPONENT ==============\n\n";
    
    std::string indexFileName = "index.bin";
    std::fstream indexFile{indexFileName, std::ios::binary | std::ios::in};
    
    if (!indexFile.is_open()) {
        answer << "ERROR: Could not open index file for analysis\n\n";
        return;
    }
    
    indexFile.seekg(0, std::ios::end);
    std::streampos fileSize = indexFile.tellg();
    NUMBER_OF_NODES_BPLUSTREE = fileSize / BLOCK_SIZE;
    indexFile.seekg(0, std::ios::beg);
    
    unsigned rootNodeId = im.rootOffset;
    
    IndexView firstNode(indexFile, 0);
    BPLUSTREE_N = (BLOCK_SIZE - sizeof(unsigned)) / sizeof(IndexEntry);
    
    NUMBER_OF_LEVELS_BPLUSTREE = 0;
    unsigned currentNodeId = rootNodeId;
    bool reachedLeaf = false;
    
    while (!reachedLeaf) {
        NUMBER_OF_LEVELS_BPLUSTREE++;
        
        IndexView nodeView(indexFile, currentNodeId);
        IndexEntry firstEntry = nodeView[0];
        
        if (firstEntry.isLeaf) {
            reachedLeaf = true;
        } else {
            currentNodeId = firstEntry.offset;
        }
        
        if (NUMBER_OF_LEVELS_BPLUSTREE > 10) {
            answer << "WARNING: Tree depth exceeds expected limits. Possible loop detected.\n";
            break;
        }
    }
    
    answer << "Root Node Content Analysis:\n";
    IndexView rootView(indexFile, rootNodeId);
    
    std::vector<float> rootKeys;
    answer << "  Raw entries in root node:\n";
    
    for (unsigned i = 0; i < BPLUSTREE_N; i++) {
        IndexEntry entry = rootView[i];
        
        if (entry.key == 0 && entry.offset == 0 && i > 0) {
            break;
        }
        
        if (entry.key != 0.0f) {
            rootKeys.push_back(entry.key);
            answer << "  - Key: " << entry.key << ", Offset: " << entry.offset 
                  << ", IsLeaf: " << (entry.isLeaf ? "true" : "false") << "\n";
        }
    }
    
    unsigned rootBackPtr = rootView.getNodeBackPointer();
    answer << "  Root back pointer: " << rootBackPtr << "\n\n";
    
    answer << "B+ Tree Structure Summary:\n";
    answer << "- Parameter n (max entries per node): " << BPLUSTREE_N << "\n";
    answer << "- Number of Nodes: " << NUMBER_OF_NODES_BPLUSTREE << "\n";
    answer << "- Number of Levels: " << NUMBER_OF_LEVELS_BPLUSTREE << "\n";
    answer << "- Root Node Keys:";
    for (float key : rootKeys) {
        answer << " " << key;
    }
    answer << "\n\n";
    
    answer << "Node types distribution:\n";
    unsigned leafCount = 0;
    unsigned internalCount = 0;
    
    for (unsigned i = 0; i < NUMBER_OF_NODES_BPLUSTREE; i++) {
        IndexView nodeView(indexFile, i);
        IndexEntry firstEntry = nodeView[0];
        
        if (firstEntry.isLeaf) {
            leafCount++;
        } else {
            internalCount++; 
        }
    }
    
    answer << "- Leaf nodes: " << leafCount << "\n";
    answer << "- Internal nodes: " << internalCount << "\n\n";
    
    indexFile.close();
}

void Task3(std::ofstream &answer, DiskManager &dm, IndexManager &im) {
    answer << "============== TASK 3: SEARCH QUERY ==============\n\n";
    
    float lowerBound = 0.6f;
    float upperBound = 0.9f;
    
    NUMBER_OF_INDEX_BLOCKS_INDEX_ACCESS = 0;
    NUMBER_OF_DATA_BLOCKS_INDEX_ACCESS = 0;
    NUMBER_OF_DATA_BLOCKS_BRUTE_FORCE = 0;
    
    auto bplusStart = std::chrono::high_resolution_clock::now();
    
    std::vector<unsigned> addresses = im.rangeQuery(lowerBound, upperBound);
    
    
    std::vector<Record> records = dm.batchQuery(addresses);

    auto bplusEnd = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> bplusTime = bplusEnd - bplusStart;
    
    float avgFgPct = 0.0f;
    if (!records.empty()) {
        float sum = 0.0f;
        for (const Record& rec : records) {
            sum += rec.FG_PCT_HOME;
        }
        avgFgPct = sum / records.size();
    }
    
    auto bruteStart = std::chrono::high_resolution_clock::now();
    
    std::vector<Record> bruteForceRecords = dm.linearScan(lowerBound, upperBound);
    
    auto bruteEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> bruteTime = bruteEnd - bruteStart;
    
    answer << "Range Query (FG_PCT_HOME from " << lowerBound << " to " << upperBound << "):\n";
    answer << "- Number of Records Retrieved: " << records.size() << "\n";
    answer << "- Number of Index Blocks Accessed: " << NUMBER_OF_INDEX_BLOCKS_INDEX_ACCESS << "\n";
    answer << "- Number of Data Blocks Accessed: " << NUMBER_OF_DATA_BLOCKS_INDEX_ACCESS << "\n";
    answer << "- Average FG_PCT_HOME: " << std::fixed << std::setprecision(6) << avgFgPct << "\n";
    answer << "- B+ Tree Query Time: " << bplusTime.count() << " ms\n\n";
    
    answer << "Brute Force Linear Scan:\n";
    answer << "- Number of Records Retrieved: " << bruteForceRecords.size() << "\n";
    answer << "- Number of Data Blocks Accessed: " << NUMBER_OF_DATA_BLOCKS_BRUTE_FORCE << "\n";
    answer << "- Linear Scan Time: " << bruteTime.count() << " ms\n";
    answer << "- Speedup: " << (bruteTime.count() / bplusTime.count()) << "x\n\n";
}

int main() {
    std::ofstream answerFile("answer.txt", std::ios::out | std::ios::trunc); 
    if (!answerFile.is_open()) {
        std::cerr << "Failed to open answer.txt for writing" << std::endl;
        return 1;
    }
    
    answerFile << "========================================\n";
    answerFile << "SC3020 DATABASE SYSTEM PRINCIPLES\n";
    answerFile << "========================================\n\n";
    
    std::fstream gameFile("games.txt", std::ios::in);
    if (!gameFile.is_open()) {
        answerFile << "ERROR: Could not open games.txt file\n";
        answerFile.close();
        return 1;
    }
    
    DiskManager dm{};
    dm.txtToBinary(gameFile, true, "data.bin");
    gameFile.close();
    
    unsigned totalNumBlocks = dm.blkMapCount["data.bin"];
    
    Task1(answerFile, dm);
    
    std::fstream dataFile{"data.bin", std::ios::binary | std::ios::in | std::ios::out};
    IndexManager im{};
    im.createBPlusTree(dataFile, totalNumBlocks);
    
    Task2(answerFile, im);
    
    Task3(answerFile, dm, im);
    
    dataFile.close();
    answerFile.close();
    
    std::cout << "Statistics have been written to answer.txt" << std::endl;
    return 0;
}