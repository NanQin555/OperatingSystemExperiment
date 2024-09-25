#include <stdio.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int print_mode(mode_t mode)
{
    	if ((S_IRUSR & mode) == S_IRUSR) {
        	printf("r");
    	}else{
		printf("-");
	} 

    	if ((S_IWUSR & mode) == S_IWUSR) {
        	printf("w");
    	}else{
		printf("-");
	}
    	if ((S_IXUSR & mode) == S_IXUSR) {
        	printf("x");
    	}else{
		printf("-");
	}
    	if ((S_IRGRP & mode) == S_IRGRP) {
        	printf("r");
    	}else{
		printf("-");
	}
    	if ((S_IWGRP & mode) == S_IWGRP) {
        	printf("w");
    	}else{
		printf("-");
	}
    	if ((S_IXGRP & mode) == S_IXGRP) {
        	printf("x");
    	}else{
		printf("-");
	}
    	if ((S_IROTH & mode) == S_IROTH) {
        	printf("r");
    	}else{
		printf("-");
	}
    	if ((S_IWOTH & mode) == S_IWOTH) {
        	printf("w");
    	}else{
		printf("-");
	}
    	if ((S_IXOTH & mode) == S_IXOTH) {
        	printf("x");
    	}else{
		printf("-");
	}
	return 0;
}


int print_owner(uid_t uid)
{
	struct passwd *pw = getpwuid(uid);	
	if(pw == NULL)
		printf("none");
	else
		printf("%s", pw->pw_name);
	return 0;
}

int print_group(gid_t gid)
{
	struct group *gr = getgrgid(gid);
	if(gr == NULL)
		printf("none");
	else
		printf("%s", gr->gr_name);
	return 0;
}


int print_time(time_t time)
{
	struct tm *ptm = gmtime(&time);
	if(ptm == NULL)
	{
		return -1;
	}

	printf("%.4d-%.2d-%.2d %.2d:%.2d:%.2d", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	return 0;

}

int show_file(char *fileName)
{
	int                     i;
        struct stat     buf;
        char            ptr;

        if (lstat(fileName,  &buf) < 0) {
        	return -1;
        }

	if(S_ISREG(buf.st_mode))  ptr = '-';
        else if (S_ISDIR(buf.st_mode))  ptr = 'd';
        else if (S_ISCHR(buf.st_mode))  ptr = 'c';
        else if (S_ISBLK(buf.st_mode))  ptr = 'b';
        else if (S_ISFIFO(buf.st_mode)) ptr = 'p';
        else if (S_ISLNK(buf.st_mode))  ptr = 'l';
        else if (S_ISSOCK(buf.st_mode)) ptr = 's';
        else ptr = '*';
        printf("%c", ptr);
	print_mode(buf.st_mode);

	printf(" ");
	printf("%lu", buf.st_nlink);
	printf(" ");
	print_owner(buf.st_uid);
	printf(" ");
	print_group(buf.st_gid);
	
	printf(" ");
	printf("%*ld", 10, buf.st_size);
	printf(" ");
	print_time(buf.st_mtime);
	printf(" ");
	printf("%s", fileName);
	printf("\n");
	
	return 0;
}

const char *dest_path = "/home/qiuyuanjie";
#define CP_PATH_FILE "/home/source/file/cp"

extern int copy_file_dir(char *src_name, char *dest_name);

int do_copy(char *name)
{
	int ret = 0;
	char *cwd = NULL;
	char buf[1024];
	char src_name[1024];
	char dest_name[1024];

	if(name == NULL)
		return -1;

	cwd = getcwd(buf, 1024);
	memset(src_name, 0, 1024);
	memset(dest_name, 0, 1024);
	snprintf(src_name, 1023, "%s/%s", cwd, name);
	snprintf(dest_name, 1023, "%s/%s", dest_path, name);

/*	//use system
	char cmd[2048];
	memset(cmd, 0, 2048);
	snprintf(cmd,2047, "%s %s %s",  CP_PATH_FILE, src_name, dest_name);
	ret = system(cmd);
	return ret;
*/

	//use fork execute and waitpid
/*	pid_t pid = fork();
	if(pid < 0)
	{
		printf("fork process error\n");
		return -1;
	}

	if(pid == 0)
	{
		int ret =  execl(CP_PATH_FILE, CP_PATH_FILE, src_name, dest_name, NULL);
		exit(ret);
	}else{
		waitpid(pid, &ret, 0);		
	}
	return ret;
*/

	//use fork call function and waitpid
	pid_t pid = fork();
	if(pid < 0)
	{
		printf("fork process error\n");
		return -1;
	}
	if(pid == 0)
	{
		ret = copy_file_dir(src_name, dest_name);
		exit(ret);
	}else{
		waitpid(pid, &ret, 0);
	}
	return ret;
	
}
	
int main(int argc, char *argv[])
{
	DIR *dr =NULL;
	struct dirent *de = NULL;
	char *path = NULL;

	if(argc < 2)
	{
		path = ".";
	}else{
		path = argv[1];
	}

	if(chdir(path) < 0)
	{
		printf("the dir %s not exist\n", path);
		return -1;
	}
	
	dr = opendir(".");
	if(dr == NULL)
	{
		printf("open dir %s error\n", path);
		return -1;
	}
	while((de=readdir(dr)) != NULL)
	{
		if(strcmp(de->d_name, ".") == 0)
			continue;
		if(strcmp(de->d_name, "..") == 0)
			continue;
		show_file(de->d_name);
		do_copy(de->d_name);
	}
	closedir(dr);
	return 0;
}



