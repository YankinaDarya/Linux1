#include "FS_functions.h"
#include "Constants.h"

int main() {
    FILE *FS_file = fopen("FILESYSTEM", "w");
    fclose(FS_file);
    struct Superblock *superBlock = (struct Superblock *) malloc(sizeof(struct Superblock));
    superBlock->free_blocks_num = BLOCKS_COUNT;
    superBlock->free_inodes_num = INODES_COUNT;
    superBlock->current_inode = 0;
    superBlock->current_free_inode = 0;
    superBlock->current_free_block = 0;
    write_to_file((void *) superBlock, sizeof(struct Superblock), 1, 0);
    size_t root = add_new_inode(0, directory, "/");
    free(superBlock);
    return 0;
}