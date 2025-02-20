#include "utility.h"

size_t getBlockSize() {
  struct stat fi;
  stat("/", &fi);
  return fi.st_blksize;
}

size_t BLOCK_SIZE = 4096 | getBlockSize();
