#ifndef LINUX_DEFINE_H
#define LINUX_DEFINE_H

#define ANSI_COLOR_RED "\x1b[31m"

#define ANSI_COLOR_GREEN "\x1b[32m"

#define ANSI_COLOR_YELLOW "\x1b[33m"

#define ANSI_COLOR_RESET "\x1b[0m"

#include <stdint.h>
#include "FS_structures.h"

const uint16_t BLOCK_SIZE = 128;
const uint16_t INODES_COUNT = 256;
const uint16_t BLOCKS_IN_INODE = 16;
const uint16_t BLOCKS_COUNT = 4096;
const uint64_t INODES_TABLE_OFFSET = sizeof(struct Superblock);
const uint64_t DATA_BLOCKS_OFFSET = INODES_TABLE_OFFSET + (sizeof(struct Inode) * INODES_COUNT);

#endif //LINUX_DEFINE_H