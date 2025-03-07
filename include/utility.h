#ifndef UTILITY_H
#define UTILITY_H
#include <sys/stat.h>
#include <sys/types.h>

#include <string>
#include <vector>
#define debug_print_vector(x)                  \
  {                                            \
    std::cout << "[";                          \
    for (int i = 0; i < x.size(); ++i) {       \
      std::cout << x[i];                       \
      if (i < x.size() - 1) std::cout << ", "; \
    }                                          \
    std::cout << "]" << std::endl;             \
  }

#define debug_print(x)           \
  {                              \
    std::cout << x << std::endl; \
  }

using Byte = unsigned char;

extern const size_t BLOCK_SIZE;

extern unsigned NUMBER_OF_RECORDS;
extern unsigned NUMBER_OF_BLOCKS;
extern unsigned NUMBER_OF_RECORDS_PER_BLOCK;
extern unsigned BPLUSTREE_N;
extern unsigned NUMBER_OF_NODES_BPLUSTREE;
extern unsigned NUMBER_OF_LEVELS_BPLUSTREE;
extern unsigned NUMBER_OF_DATA_BLOCKS_INDEX_ACCESS;
extern unsigned NUMBER_OF_INDEX_BLOCKS_INDEX_ACCESS;
extern unsigned NUMBER_OF_DATA_BLOCKS_BRUTE_FORCE;


size_t getBlockSize();

std::vector<std::string> split(std::string input, std::string del);

#endif  // !UTILITY_H