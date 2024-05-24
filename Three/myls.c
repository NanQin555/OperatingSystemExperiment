/*  
 *  myls.c
 *  Wang Nanqin
 *  2024.4.26
 */
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <time.h>
#define LOGIN_NAME_MAX 256
#define MAX_PATH 256
void print_file_info(const char *filename, struct stat *sb);
void printTable();
int is_local(const char* filename);
int is_hide(const char* filename);
int lable_local=0;//-ll
int lable_hide=0;//-h
// int lable_recursion=0;//-r
int main(int argc, char **argv) {
    char cwd[MAX_PATH];
    getcwd(cwd, sizeof(cwd));
    char* param;
    param = cwd;
    // if (argc > 1) {
    //     param = argv[1]; // 使用命令行参数作为目录路径
    // }
    for(int i=1; i<argc; i++) {
        if(argv[i][0] == '-') {
            if(strcmp(argv[i], "-ll") == 0) lable_local=1;
            else if(strcmp(argv[i], "-h") == 0) lable_hide=1;
            else {
                perror ("Invalid option");
                _exit(1);
            }
        }
        else param = argv[i];
    }

    DIR *dir = opendir(param);
    // DIR *dir = opendir(cwd);
    if (dir == NULL) {
        perror("Failed to open directory");
        return 1;
    }

    struct dirent* entry;
    printTable();
    while ((entry = readdir(dir)) != NULL) {
        char path[MAX_PATH];
        snprintf(path, sizeof(path), "%s/%s", param, entry->d_name);

        struct stat statbuffer;
        if (stat(path, &statbuffer) == -1) {
            perror("Failed to stat file");
            continue;
        }
        if(is_local(entry->d_name)) {
            if(lable_local==1) {
                print_file_info(entry->d_name, &statbuffer);
            }
            else continue;
        }
        else if (is_hide(entry->d_name)) {
            if(lable_hide==1) {
                print_file_info(entry->d_name, &statbuffer);
            }
            else continue;
        }
        else print_file_info(entry->d_name, &statbuffer);
    }
    closedir(dir);
    return 0;
}

void print_file_info(const char *filename, struct stat *sb) {
    // 文件类型
    mode_t mode = sb->st_mode;
    char filetype[10];
    if (S_ISREG(mode)) {
        strcpy(filetype, "REG ");
    } else if (S_ISDIR(mode)) {
        strcpy(filetype, "DIR ");
    } else if (S_ISLNK(mode)) {
        strcpy(filetype, "MOD ");
    } else if (S_ISCHR(mode)) {
        strcpy(filetype, "LNK ");
    } else if (S_ISBLK(mode)) {
        strcpy(filetype, "BLK ");
    } else if (S_ISFIFO(mode)) {
        strcpy(filetype, "FIFO");
    } else if (S_ISSOCK(mode)) {
        strcpy(filetype, "SOCK");
    } else {
        strcpy(filetype, "ERROR!");
        // printf("Error file type\n");
    }

    // 文件权限
    char permissions[11];
    permissions[0] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[3] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[6] = (mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (mode & S_IXOTH) ? 'x' : '-';
    permissions[9] = '\0';

    // 文件硬链接数
    unsigned long nlinks = sb->st_nlink;

    // 文件所有者用户名
    struct passwd *pw = getpwuid(sb->st_uid);
    char owner[LOGIN_NAME_MAX];
    if (pw) {
        strncpy(owner, pw->pw_name, LOGIN_NAME_MAX);
        owner[LOGIN_NAME_MAX - 1] = '\0';
    } else {
        snprintf(owner, sizeof(owner), "%lu", (unsigned long)sb->st_uid);
    }

    // 文件所有者所在组用户名
    struct group *gr = getgrgid(sb->st_gid);
    char group[LOGIN_NAME_MAX];
    if (gr) {
        strncpy(group, gr->gr_name, LOGIN_NAME_MAX);
        group[LOGIN_NAME_MAX - 1] = '\0';
    } else {
        snprintf(group, sizeof(group), "%lu", (unsigned long)sb->st_gid);
    }

    // 文件大小（以字节为单位）
    off_t size = sb->st_size;

    // 文件最后修改时间
    time_t mtime = sb->st_mtime;
    struct tm *mtime_tm = localtime(&mtime);
    char mtime_str[32];
    strftime(mtime_str, sizeof(mtime_str), "%Y-%m-%d %H:%M:%S", mtime_tm);


    printf("%s\t %s\t%lu\t%s\t%s\t%ld\t%s\t%s\n",
           filetype, permissions, nlinks,
           owner, group, (long)size, mtime_str, filename);
}

void printTable() {
    printf("FileType Permissions\tLinks\tOwner\tGroup\tSize\tLastModified\t\tFilename\n");
}

int is_local(const char* filename) {
    if(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) return 1;
    else return 0;
}

int is_hide(const char* filename) {
    if(filename[0] == '.'  && (filename[1] != '.'||filename[1] != '\0')) return 1;
    else return 0;
}