#ifndef UTILITY_H
#define UTILITY_H
#include <sys/stat.h>
#include <sys/types.h>

#define print(x)                 \
  {                              \
    std::cout << x << std::endl; \
  }

using Byte = unsigned char;

extern size_t BLOCK_SIZE;
extern size_t RECORD_SIZE;

size_t getBlockSize();

#endif  // !UTILITY_H