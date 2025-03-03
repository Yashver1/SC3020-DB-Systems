#include <record.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "block.h"

int main() {

  std::ifstream file{"../games.txt"};
  std::vector<std::string> buffer{};
  if (!file.is_open()) {
    return 1;
  }
  
  std::string temp{};
  while (std::getline(file, temp)) {
    buffer.push_back(temp);
  }


  file.close();
  return 0;
}