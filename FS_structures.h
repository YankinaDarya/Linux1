#ifndef FILE_SYSTEM_BASE_STRUCTURES_H
#define FILE_SYSTEM_BASE_STRUCTURES_H

#include <stdlib.h>
#include <stdio.h>

struct Superblock {
    size_t free_blocks_num;
    size_t free_inodes_num;
    size_t current_free_inode;
    size_t current_free_block;
    size_t current_inode;
};

typedef enum { directory, file } inode_type;

struct Inode {
    size_t inode_num;
    size_t parent_inode_num;
    inode_type type;
    size_t size;
    char file_name[32];
    size_t blocks_nums[16];
};

struct Directory_info {
    size_t inode_num;
    size_t parent_inode_num;
    char file_name[32];
};

#endif //FILE_SYSTEM_BASE_STRUCTURES_H