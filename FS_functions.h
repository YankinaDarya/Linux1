#ifndef LINUX_FS_BASE_H
#define LINUX_FS_BASE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "FS_structures.h"
#include "Constants.h"

struct Inode *get_inode_num(size_t num);

size_t get_files_number(struct Inode *inode);

void *write_to_filesystem(size_t num, size_t size, void *info);

void *read_from_filesystem(size_t num);

size_t add_new_inode(size_t parent_num, inode_type type, char *file_name);

void read_from_file(void *info, size_t size, size_t nums, const uint64_t offset);

void write_to_file(void *info, size_t size, size_t nums, const uint64_t offset);

void ls();

void add_new_item(char *file_name, inode_type type); // touch and mkdir

void cd(char *dir_name);

void echo(char *, char *);

void cat(char *);

void help();

struct Inode *get_inode_num(size_t num) {
    uint64_t offset = INODES_TABLE_OFFSET + num * sizeof(struct Inode);
    struct Inode *inode = (struct Inode *) malloc(sizeof(struct Inode));
    read_from_file(inode, sizeof(struct Inode), 1, offset);
    return inode;
}

size_t get_files_number(struct Inode *inode) {
    return (size_t) (inode->size / sizeof(struct Directory_info));
}

void *write_to_filesystem(size_t num, size_t size, void *info) {
    struct Inode *node = get_inode_num(num);
    node->size = size;
    size_t ptr = 0;
    for (size_t i = 0; i < BLOCKS_IN_INODE && ptr < size; i++) {
        size_t current_block = node->blocks_nums[i];
        size_t writing_size;
        if (size - ptr > BLOCK_SIZE) {
            writing_size = BLOCK_SIZE;
        } else {
            writing_size = size - ptr;
        }
        void *data_to_write = malloc(writing_size);
        memcpy(data_to_write, info + ptr, writing_size);
        uint64_t offset = DATA_BLOCKS_OFFSET + current_block * BLOCK_SIZE;
        write_to_file(data_to_write, BLOCK_SIZE, 1, offset);
        ptr += writing_size;
        free(data_to_write);
    }
    uint64_t offset = INODES_TABLE_OFFSET + node->inode_num * sizeof(struct Inode);
    write_to_file(node, sizeof(struct Inode), 1, offset);
    free(node);
}

void *read_from_filesystem(size_t num) {
    struct Inode *node = get_inode_num(num);
    size_t file_size = node->size;
    size_t ptr = 0;
    void *output = malloc(file_size);
    for (size_t i = 0; ptr < file_size; i++) {
        size_t current_block = node->blocks_nums[i];
        size_t reading_size;
        if (file_size - ptr > BLOCK_SIZE) {
            reading_size = BLOCK_SIZE;
        } else {
            reading_size = file_size - ptr;
        }
        void *data_to_read = malloc(reading_size);
        uint64_t offset = DATA_BLOCKS_OFFSET + current_block * BLOCK_SIZE;
        read_from_file(data_to_read, BLOCK_SIZE, 1, offset);
        memcpy(output + ptr, data_to_read, reading_size);
        ptr += reading_size;
        free(data_to_read);
    }
    return output;
}

size_t add_new_inode(size_t parent_num, inode_type type, char *file_name) {
    struct Superblock *superblock = (struct Superblock *) malloc(sizeof(struct Superblock));
    read_from_file(superblock, sizeof(struct Superblock), 1, 0);
    if (superblock->free_inodes_num <= 0 || superblock->free_blocks_num < BLOCKS_IN_INODE) {
        printf(ANSI_COLOR_RED"No free inodes or blocks available\n"ANSI_COLOR_RESET);
    }
    size_t num = superblock->current_free_inode;
    superblock->current_free_inode++;
    size_t *free_blocks = (size_t *) malloc(BLOCKS_IN_INODE * sizeof(size_t));
    for (int i = 0; i < BLOCKS_IN_INODE; i++) {
        free_blocks[i] = superblock->current_free_block;
        superblock->current_free_block++;
        superblock->free_blocks_num--;
    }
    struct Inode *node = (struct Inode *) malloc(sizeof(struct Inode));
    for (size_t i = 0; i < BLOCKS_IN_INODE; i++) {
        node->blocks_nums[i] = free_blocks[i];
    }
    superblock->free_inodes_num--;
    node->inode_num = num;
    node->parent_inode_num = parent_num;
    node->type = type;
    node->size = 0;
    strncat(node->file_name, file_name, sizeof(file_name));
    uint64_t offset = INODES_TABLE_OFFSET + node->inode_num * sizeof(struct Inode);
    write_to_file(node, sizeof(struct Inode), 1, offset);
    write_to_file((void *) superblock, sizeof(struct Superblock), 1, 0);
    if (type == directory) {
        struct Directory_info dir_info[1];
        strcpy(dir_info[0].file_name, ".");
        dir_info[0].inode_num = num;
        dir_info[0].parent_inode_num = parent_num;
        write_to_filesystem(num, sizeof(struct Directory_info), (void *) dir_info);
    }
    free(node);
    free(superblock);
    return num;
}

void read_from_file(void *info, size_t size, size_t nums, const uint64_t offset) {
    FILE *file = fopen("FILESYSTEM", "r+");
    if (file == NULL) {
        exit(1);
    }
    fseek(file, offset, SEEK_SET);
    fread(info, size, nums, file);
    fclose(file);
}

void write_to_file(void *info, size_t size, size_t nums, const uint64_t offset) {
    FILE *file = fopen("FILESYSTEM", "r+");
    if (file == NULL) {
        exit(1);
    }
    fseek(file, offset, SEEK_SET);
    fwrite(info, size, nums, file);
    fclose(file);
}

void ls() {
    struct Superblock *superblock = (struct Superblock *) malloc(sizeof(struct Superblock));
    read_from_file(superblock, sizeof(struct Superblock), 1, 0);
    struct Inode *inode = get_inode_num(superblock->current_inode);
    struct Directory_info *directory_info = (struct Directory_info *) read_from_filesystem(inode->inode_num);
    size_t files_number = get_files_number(inode);
    for (size_t i = 1; i < files_number; i++) {
        struct Inode *file = get_inode_num(directory_info[i].inode_num);
        if (file->type == directory) {
            printf(ANSI_COLOR_YELLOW"%s "ANSI_COLOR_RESET, directory_info[i].file_name);
        } else {
            printf("%s ", directory_info[i].file_name);
        }
        free(file);
    }
    free(directory_info);
    free(superblock);
    free(inode);
    printf("\n");
}

void add_new_item(char *file_name, inode_type type) {
    struct Superblock *superblock = (struct Superblock *) malloc(sizeof(struct Superblock));
    read_from_file(superblock, sizeof(struct Superblock), 1, 0);
    struct Inode *node = get_inode_num(superblock->current_inode);
    struct Directory_info *directory_info = (struct Directory_info *) read_from_filesystem(node->inode_num);
    size_t files_number = get_files_number(node);
    for (size_t i = 0; i < files_number; ++i) {
        if (strcmp(directory_info[i].file_name, file_name) == 0) {
            printf(ANSI_COLOR_RED"File is already exists\n"ANSI_COLOR_RED);
            return;
        }
    }
    size_t new_size = (files_number + 1) * sizeof(struct Directory_info);
    directory_info = realloc(directory_info, new_size);
    size_t new_inode = add_new_inode(node->inode_num, type, file_name);
    strcpy(directory_info[files_number].file_name, file_name);
    directory_info[files_number].inode_num = new_inode;
    write_to_filesystem(node->inode_num, new_size, (void *) directory_info);
    free(superblock);
    free(node);
    free(directory_info);
}

void cd(char *dir_name) {
    struct Superblock *superblock = (struct Superblock *) malloc(sizeof(struct Superblock));
    read_from_file(superblock, sizeof(struct Superblock), 1, 0);
    struct Inode *inode = get_inode_num(superblock->current_inode);
    struct Directory_info *directory_info = (struct Directory_info *) read_from_filesystem(inode->inode_num);
    if (strcmp(dir_name, "..") == 0 && superblock->current_inode != 0) {
        superblock->current_inode = directory_info[0].parent_inode_num;
        write_to_file((void *) superblock, sizeof(struct Superblock), 1, 0);
        free(inode);
        free(directory_info);
        free(superblock);
        return;
    }
    size_t files_number = get_files_number(inode);
    for (size_t i = 1; i < files_number; i++) {
        if (strcmp(directory_info[i].file_name, dir_name) == 0) {
            struct Inode *directory = get_inode_num(directory_info[i].inode_num);
            if (directory->type == file) {
                printf(ANSI_COLOR_RED"Not a directory\n"ANSI_COLOR_RESET);
                free(inode);
                free(directory_info);
                free(superblock);
                return;
            }
            superblock->current_inode = directory_info[i].inode_num;
            write_to_file((void *) superblock, sizeof(struct Superblock), 1, 0);
            free(directory);
            free(inode);
            free(directory_info);
            free(superblock);
            return;
        }
    }
    free(inode);
    free(directory_info);
    free(superblock);
}

void echo(char *file_name, char *str) {
    struct Superblock *superblock = (struct Superblock *) malloc(sizeof(struct Superblock));
    read_from_file(superblock, sizeof(struct Superblock), 1, 0);
    struct Inode *current_inode = get_inode_num(superblock->current_inode);
    struct Directory_info *directory_info = (struct Directory_info *) read_from_filesystem(current_inode->inode_num);
    size_t files_number = get_files_number(current_inode);
    for (size_t i = 0; i < files_number; ++i) {
        if (strcmp(file_name, directory_info[i].file_name) == 0) {
            struct Inode *file = get_inode_num(directory_info[i].inode_num);
            if (file->type == directory) {
                printf(ANSI_COLOR_RED"Not a file %s\n"ANSI_COLOR_RESET, file_name);
                free(current_inode);
                free(directory_info);
                free(superblock);
                return;
            }
            write_to_filesystem(file->inode_num, (strlen(str) + 1) * sizeof(char), str);
            free(file);
            free(current_inode);
            free(directory_info);
            free(superblock);
            return;
        }
    }
    free(current_inode);
    free(directory_info);
    free(superblock);
}

void cat(char *file_name) {
    struct Superblock *superblock = (struct Superblock *) malloc(sizeof(struct Superblock));
    read_from_file(superblock, sizeof(struct Superblock), 1, 0);
    struct Inode *current_inode = get_inode_num(superblock->current_inode);
    struct Directory_info *directory_info = (struct Directory_info *) read_from_filesystem(current_inode->inode_num);
    size_t files_number = get_files_number(current_inode);
    for (size_t i = 0; i < files_number; ++i) {
        if (strcmp(file_name, directory_info[i].file_name) == 0) {
            struct Inode *file = get_inode_num(directory_info[i].inode_num);
            if (file->type == directory) {
                printf(ANSI_COLOR_RED"Not a file\n"ANSI_COLOR_RESET);
                free(current_inode);
                free(directory_info);
                free(superblock);
                return;
            }
            char *str = read_from_filesystem(file->inode_num);
            free(file);
            printf("%s\n", str);
            free(str);
            free(current_inode);
            free(directory_info);
            free(superblock);
            return;
        }
    }
    free(current_inode);
    free(directory_info);
    free(superblock);
}

void help() {
    printf("Possible commands:\n");
    printf("mkdir <directory name>\n");
    printf("touch <file name>\n");
    printf("ls\n");
    printf("echo <string> <file name>\n");
    printf("cat <file name>\n");
    printf("cd <directory name (or ..)>\n");
    printf("help <file name>\n");
    printf("exit\n");
}

#endif //LINUX_FS_BASE_H