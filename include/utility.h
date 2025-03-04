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

size_t getBlockSize();

std::vector<std::string> split(std::string input, std::string del);

#endif  // !UTILITY_H