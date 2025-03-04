#include "utility.h"

#include <sstream>

size_t getBlockSize() {
  struct stat fi;
  stat("/", &fi);
  return fi.st_blksize;
}

std::vector<std::string> split(std::string input, std::string del) {
  std::vector<std::string> tokens{};
  size_t pos = 0;
  std::string token{};
  while ((pos = input.find(del)) != std::string::npos) {
    token = input.substr(0, pos);
    tokens.push_back(token);
    input.erase(0, pos + del.length());
  }
  tokens.push_back(input);
  return tokens;
}

const size_t BLOCK_SIZE = getBlockSize();
