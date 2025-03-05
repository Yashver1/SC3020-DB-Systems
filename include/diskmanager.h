#ifndef DISKMANAGER_H
#define DISKMANAGER_H
#define DISKMANAGER_H
#include "fstream"
#include "string"
#include "unordered_map"
class DiskManager {
 public:
  std::unordered_map<std::string, unsigned> blkMapCount;
  DiskManager() : blkMapCount{} {};
  void txtToBinary(std::fstream &input, bool header,
                   std::string name = "data.bin");
  void linearScan(float lowerBound, float upperBound,
                  std::string name = "data.bin");
};

#endif