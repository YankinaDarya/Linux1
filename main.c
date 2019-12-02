#include "FS_functions.h"

int main() {
    help();
    printf(ANSI_COLOR_GREEN"FILESYSTEM$ "ANSI_COLOR_RESET);
    char action[8];
    char arg1[64];
    char arg2[64];
    char str[128];
    while(fgets(str, 128, stdin) != 0) {
        sscanf(str, "%s %s %s", action, arg1, arg2);
        if (strcmp(action, "mkdir") == 0) {
            add_new_item(arg1, directory);
        }
        else if (strcmp(action, "touch") == 0) {
            add_new_item(arg1, file);
        }
        else if (strcmp(action, "ls") == 0) {
            ls();
        }
        else if (strcmp(action, "cd") == 0) {
            cd(arg1);
        }
        else if (strcmp(action, "cat") == 0) {
            cat(arg1);
        }
        else if (strcmp(action, "echo") == 0) {
            size_t sizeOfInput = sizeof(char) * strlen(arg1);
            char* text = (char*)malloc(sizeOfInput);
            strcpy(text, arg1);
            echo(arg2, text);
            free(text);
        }
        else if (strcmp(action, "help") == 0) {
            help();
        }
        else if (strcmp(action, "exit") == 0) {
            return 0;
        }
        else {
            printf(ANSI_COLOR_RED"Wrong command\n"ANSI_COLOR_RESET);
        }
        printf(ANSI_COLOR_GREEN"FILESYSTEM$ "ANSI_COLOR_RESET);
    }
}