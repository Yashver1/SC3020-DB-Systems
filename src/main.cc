#include <record.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "block.h"

int main() {
  std::fstream file{"../games.txt", std::ios::in};
  std::vector<std::string> buffer{};

  if (!file.is_open()) {
    return 1;
  }

  std::string temp{};
  while (std::getline(file, temp)) {
    buffer.push_back(temp);
  }

  print(buffer[0]);
  print(buffer[1]);

  file.close();
  return 0;
}