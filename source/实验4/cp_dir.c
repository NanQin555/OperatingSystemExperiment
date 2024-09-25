#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int copy_file(char *src_file, char *dest_file)
{
        int rfd, wfd, n;
        char buf[1024];

        if((src_file == NULL) || (dest_file == NULL))
        {
                printf("please give source file and dest file\n");
                return -1;
        }

        rfd = open(src_file, O_RDONLY);
        if(rfd < 0)
        {
                printf("open read file %s error\n", src_file);
                return -1;
        }
        wfd = open(dest_file, O_WRONLY|O_CREAT);
        if(wfd < 0)
        {
                printf("open write file %s error\n", dest_file);
                return -1;
        }
        while(n=read(rfd, buf, 1024)>0)
        {
                if(write(wfd, buf, n) != n)
                {
                        printf("write file error\n");
                        return -1;
                }
        }
        if(n < 0)
                printf("read source file error\n");
        close(rfd);
        close(wfd);
        return 0;
}

int copy_folder(char* source_path,char *destination_path)
{
        struct dirent* filename;
	int ret = 0;

        DIR* des_dir =  opendir(destination_path);
        if(!des_dir)
        {
                if (mkdir(destination_path,0777))
                {
                        printf("创建文件夹失败！");
                        return -1;
                }
        }
        closedir(des_dir);

        DIR* dp=opendir(source_path);
        if(dp == NULL)
        {
                printf("open source dir error\n");
                return -1;
        }

        while((filename = readdir(dp)) != NULL)
	{
                char file_source_path[1024] = {0};
                snprintf(file_source_path,sizeof(file_source_path),"%s/%s",source_path,filename->d_name);

                char file_destination_path[1024] = {0};
                snprintf(file_destination_path,sizeof(file_destination_path),"%s/%s",destination_path,filename->d_name);
                if(filename->d_type == DT_DIR)
                {
                        if((strcmp(filename->d_name,".") != 0 )&& (strcmp(filename->d_name,"..") != 0 ))
                        {
                                copy_folder(file_source_path,file_destination_path);
                        }
                }
                else{   
                        if(filename->d_type == DT_REG) 
                        {       
                                ret = copy_file(file_source_path,file_destination_path);  
				if(ret == 0)
                              	    printf("复制%s到%s成功！\n",file_source_path,file_destination_path);  
                        }       
                }
        }

        closedir(dp);
}

int copy_file_dir(char *src_name, char *dest_name)
{
	struct stat st;
        char dest[1024];
        char *last_name = NULL;

        if(lstat(src_name, &st) != 0)
        {
                printf("the source file or dir not exist\n");
                return -1;
        }
        if(S_ISREG(st.st_mode))
        {
                if((lstat(dest_name, &st) == 0) && (S_ISDIR(st.st_mode)))
                {
                        memset(dest, 0, 1024);
                        last_name = strrchr(src_name, '/');
                        if(last_name == NULL)
                                last_name = src_name;
                        else
                                last_name++;
                        sprintf(dest, "%s/%s", dest_name, last_name);
                        copy_file(src_name, dest);
                }else{
                        copy_file(src_name, dest_name);
                }
        }else{
                copy_folder(src_name, dest_name);
	}
}
