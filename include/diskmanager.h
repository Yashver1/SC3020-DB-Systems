#ifndef DISKMANAGER_H
#define DISKMANAGER_H
#include "fstream"
#include "unordered_map"
#include "string"
class DiskManager {
 public:
  std::unordered_map<std::string,unsigned> blkMapCount;
  DiskManager(): blkMapCount{} {};
  void txtToBinary(std::fstream &input, bool header, std::string name = "data.bin");
  void linearScan(std::string name = "data.bin");
};

#endif