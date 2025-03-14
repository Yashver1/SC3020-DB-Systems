#ifndef DISKMANAGER_H
#define DISKMANAGER_H
#define DISKMANAGER_H
#include "fstream"
#include "string"
#include "unordered_map"
#include "record.h"

class DiskManager {
 public:
  std::unordered_map<std::string, unsigned> blkMapCount;
  DiskManager() : blkMapCount{} {};
  void txtToBinary(std::fstream &input, bool header,
                   std::string name = "data.bin");
  std::vector<Record> linearScan(float lowerBound, float upperBound,
                  std::string name = "data.bin");

  Record query( unsigned address, std::string name = "data.bin");
  std::vector<Record> batchQuery(std::vector<unsigned> &addresses, std::string name = "data.bin");
};

//Byte level offset -> actl record

#endif