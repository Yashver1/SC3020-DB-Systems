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

unsigned NUMBER_OF_RECORDS = 0;
unsigned NUMBER_OF_BLOCKS = 0;
unsigned NUMBER_OF_RECORDS_PER_BLOCK = 0;
unsigned BPLUSTREE_N = 0;
unsigned NUMBER_OF_NODES_BPLUSTREE = 0;
unsigned NUMBER_OF_LEVELS_BPLUSTREE = 0;
unsigned NUMBER_OF_DATA_BLOCKS_INDEX_ACCESS = 0;
unsigned NUMBER_OF_INDEX_BLOCKS_INDEX_ACCESS = 0;
unsigned NUMBER_OF_DATA_BLOCKS_BRUTE_FORCE = 0;

const size_t BLOCK_SIZE = getBlockSize();
