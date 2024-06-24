/*  
 *  mycp.c
 *  Wang Nanqin
 *  2024.5.10
 */
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#define MAX_PATH 512
#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];
int lable_recursio = 0;

void cpfolder(char* source_path, char* target_path);
void cpfile(char* source_path, char* target_path);
void cpfile_text(int source, int target);
int main(int argc, char** argv) {
    char cwd[MAX_PATH];
    char* source_path = NULL;
    char* target_path = NULL;
    getcwd(cwd, sizeof(cwd));
    char* param = argv[0];
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

    // sources file or directory
    struct stat source_statbuf;
    if(stat(source_path, &source_statbuf) == -1) perror("Source file not found");
    struct stat target_statbuf;
    if(stat(target_path, &target_statbuf) == -1) perror("Target not found");

    if(S_ISDIR(source_statbuf.st_mode)) {
        if(S_ISREG(target_statbuf.st_mode)) {
            perror("Wrong Target address");
            return -1;
        }
        cpfolder(source_path, target_path);
    }
    else if (S_ISREG(source_statbuf.st_mode)) {
        cpfile(source_path, target_path);
    }
    return 0;
}

void cpfolder(char* source_path, char* target_path) {
    DIR* source_dir = opendir(source_path);
    if (source_dir == NULL) perror("Failed to open dir");

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
        else {
            perror("Failed to create target folder");
            _exit(-1);
        }
    }

    struct dirent* entry;
    while ((entry = readdir(source_dir)) != NULL) {
        // entry->d_type == DT_REG;
        // entry->d_type == DT_DIR;
        char _sourcepath[MAX_PATH];
        snprintf(_sourcepath, sizeof(_sourcepath), "%s/%s", source_path, entry->d_name);
        struct stat source_statbuf;
        if(stat(_sourcepath, &source_statbuf) == -1) perror("Source file not found");
        if(S_ISDIR(source_statbuf.st_mode) && (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)) {
            // target directory
            if(lable_recursio == 1) {
                cpfolder(_sourcepath, _targetpath);
            }
            else continue;
        }
        else if (S_ISREG(source_statbuf.st_mode)) {
            cpfile(_sourcepath, _targetpath);
        }
        else {
            // perror("cpfolder error");   
        }
    }
    closedir(source_dir);
}

void cpfile(char* source_path, char* target_path) {
    int source = open(source_path, O_RDONLY);
    if (source < 0) perror("Source file not found");

    char* last_slash = strrchr (source_path, '/');
    if(last_slash != NULL) {
        last_slash++;
    }
    char _targetpath[MAX_PATH];
    snprintf(_targetpath, sizeof(_targetpath), "%s/%s", target_path, last_slash);

    // int target = open(_targetpath, O_WRONLY | O_CREAT, 0644);
    int target = open(_targetpath, O_WRONLY);
    if (target < 0) {
        target = open(_targetpath, O_WRONLY | O_CREAT, 0644);
        cpfile_text(source, target);
    }
    else {
        printf("\nthe Target file: %s is Existed\n",_targetpath);
        while(1) {
            printf("Enter: R(ecover), A(ppend)\n");

            char choice;
            scanf("%c", &choice);
            int c;
            do {
                c = getchar();
            } while (c != '\n' && c != EOF);

            switch(choice) {
                case 'R':
                case 'r':
                    printf("Recover the target file\n");
                    cpfile_text(source, target);
                    close(target);
                    break;
                case 'A':
                case 'a':
                    close(target);
                    target = open(_targetpath, O_WRONLY | O_APPEND);
                    printf("Append the target file\n");
                    cpfile_text(source, target);
                    close(target);
                    break;
                default:
                    printf("Invalid choice\n");
                    // break;
            }
            if(choice == 'r' || choice == 'a' || choice == 'R' || choice == 'A') 
                {break;}
        }
    }
    close(target);
    close(source);
}

void cpfile_text(int source, int target) {
    ssize_t bytesRead;
    while ((bytesRead = read(source, buffer, BUFFER_SIZE)) > 0) {
        if(write(target, buffer, bytesRead) != bytesRead) {
            perror("Error writing to target file");
        }
    }
    if (bytesRead < 0) {
        perror("Error reading from source file");
    }
}