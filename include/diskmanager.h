#ifndef DISKMANAGER_H
#define DISKMANAGER_H
#include "fstream"
class DiskManager {
 public:
  DiskManager() {};
  void txtToBinary(std::fstream &input, bool header);
};

#endif