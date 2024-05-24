/*  
 *  mycp_p.c
 *  Wang Nanqin
 *  2024.5.11
 */
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pwd.h>
#include <string.h>
#include <errno.h>
#define LOGIN_NAME_MAX 256
#define MAX_PATH 256
int lable_recursio = 0;
void Read_Dir(const char* source, const char* target);

int main(int argc, char **argv) {
    char* source_path = NULL;
    char* target_path = NULL;

    if(argc==4) {
        for(int i=1; i<argc; i++) {
            if(argv[i][0] == '-') {
                if(strcmp(argv[i], "-r") == 0) {
                    lable_recursio = 1;
                    continue;
                }
                else {
                    perror("Argv wrong");
                    _exit(-1);
                }
            }
            else if(source_path == NULL) {
                source_path = argv[i];
                continue;
            }
            else if (target_path == NULL) {
                target_path = argv[i];
                continue;
            }
            else perror("Argv wrong");
        }
    }
    else if(argc == 3) {
        source_path = argv[1];
        target_path = argv[2];
    }
    else perror("Argc wrong");

    struct stat statbuffer;
    if (stat(source_path, &statbuffer) == -1) {
        perror("Failed to stat file");
    }

    if(S_ISDIR(statbuffer.st_mode)) {
        char* last_slash = strrchr (source_path, '/');
        if(last_slash != NULL) {
            last_slash++;
        }
        else {
            perror("cpfolder error");
        }
        char _targetpath[MAX_PATH];
        snprintf(_targetpath, sizeof(_targetpath), "%s/%s", target_path, last_slash);
        if(mkdir(_targetpath, 0755) == -1) {
            if(errno == EEXIST) {}
            else perror("Failed to create target folder");
        }
        Read_Dir(source_path, _targetpath);
    }
    else if (S_ISREG(statbuffer.st_mode)) {
        pid_t pid = fork();
        if(pid == 0) {
            if(lable_recursio==1) {
                if(execl("/home/nanqin/OperatingSystems/Experiment/Four/mycp", "mycp", "-r",source_path, target_path, NULL)) {
                    perror("Failed to execute mycp");
                    _exit(EXIT_FAILURE);
                }
            }
            else {
                if(execl("/home/nanqin/OperatingSystems/Experiment/Four/mycp", "mycp", source_path, target_path, NULL)) {
                    perror("Failed to execute mycp");
                    _exit(EXIT_FAILURE);
                }
            }
        }
        else if(pid < 0) {
            perror("Failed to fork");
            exit(1);
        }
        else {
            wait(NULL);
        }
    }
    else perror("Wrong source");

    return 0;
}
void Read_Dir(const char* source, const char* target) {
    if(lable_recursio==0) {
        return;
    }

    // source directory
    DIR *dir = opendir(source);
    if (dir == NULL) {
        perror("Failed to open directory");
        // return -1;
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        char source_path[MAX_PATH];
        snprintf(source_path, sizeof(source_path), "%s/%s", source, entry->d_name);
        struct stat statbuffer;
        if (stat(source_path, &statbuffer) == -1) {
            perror("Failed to stat file");
            continue;
        }
        else {
            mode_t mode =statbuffer.st_mode;
            if(S_ISREG(mode)) {
                pid_t pid = fork();
                if(pid == 0) {
                    if(lable_recursio==1) {
                        if(execl("/home/nanqin/OperatingSystems/Experiment/Four/mycp", "mycp", "-r",source_path, target, NULL)) {
                            perror("Failed to execute mycp");
                            _exit(EXIT_FAILURE);
                        }
                    }
                    else {
                        if(execl("/home/nanqin/OperatingSystems/Experiment/Four/mycp", "mycp", source_path, target, NULL)) {
                            perror("Failed to execute mycp");
                            _exit(EXIT_FAILURE);
                        }
                    }
                }
                else if(pid < 0) {
                    perror("Failed to fork");
                    exit(1);
                }
                else {
                    wait(NULL);
                }
            }
            else if (S_ISDIR(mode) && (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)) {
                char* last_slash = strrchr (source_path, '/');
                if(last_slash != NULL) {
                    last_slash++;
                }
                else {
                    perror("cpfolder error");
                }
                char target_path[MAX_PATH];
                snprintf(target_path, sizeof(target_path), "%s/%s", target, last_slash);

                if(mkdir(target_path, 0755) == -1) {
                    if(errno == EEXIST) {}
                    else perror("Failed to create target folder");
                }
                Read_Dir(source_path, target_path);
            }
        }
    }
    closedir(dir);
}
