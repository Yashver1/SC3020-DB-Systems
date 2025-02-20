#ifndef UTILITY_H
#define UTILITY_H
#include <sys/stat.h>
#include <sys/types.h>

using byte = unsigned char;

extern size_t BLOCK_SIZE;
extern size_t RECORD_SIZE;

size_t getBlockSize();

#endif  // !UTILITY_H