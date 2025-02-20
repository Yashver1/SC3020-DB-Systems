#include <record.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "block.h"

int main() {
  Record rec{};
  RecordHeader h{};
  std::cout << sizeof(h) << std::endl;
  std::cout << sizeof(rec) << std::endl;
  std::cout << rec << std::endl;
  return 0;
}