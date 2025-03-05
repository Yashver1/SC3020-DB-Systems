#include <record.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <diskmanager.h>
#include "block.h"

int main() {
  std::fstream inputFile{"games.txt", inputFile.out | inputFile.in};
  DiskManager dm{};
  dm.txtToBinary(inputFile, true);

  inputFile.close();
}