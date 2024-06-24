#include "yuv_to_jpeg.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#define BUFFCOUNT 3
#define WIDTH 640
#define HEIGHT 480
int main() {
    int cameraFd = open("/dev/video1", O_RDWR);//|O_NONBLOCK

    // 查询设备能力
    struct v4l2_capability cap;
    memset(&cap, 0, sizeof(cap));
    int iret = ioctl(cameraFd, VIDIOC_QUERYCAP, &cap);
    if(iret < 0) {
        perror("get vidieo capabilityerror");
        // return;
    }
    printf("DriveName: %s\nCard name:%s\nBufinfo:%s\nDriverVersion:%u.%u.%u\n", \
    cap.driver, cap.card, cap.bus_info, (cap.version>>16)&0XFF, (cap.version>>8)&0XFF, cap.version&0XFF);

    // 设置摄像头参数
    // struct v4l2_standard std_id;
    // ioctl(cameraFd, VIDIOC_S_STD, &std_id);
    // struct v4l2_crop crop;
    // ioctl(cameraFd, VIDIOC_S_CROP, &crop);

    struct v4l2_format fmt;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    // fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field=V4L2_FIELD_INTERLACED;
    iret = ioctl(cameraFd, VIDIOC_S_FMT, &fmt); 
    if(iret < 0) {
        perror("set video format error");
        // return;
    }
    // struct v4l2_streamparm parm;
    // parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    // parm.parm.capture.timeperframe.numerator = 1;
    // parm.parm.capture.timeperframe.denominator = 30;
    // ioctl(cameraFd, VIDIOC_S_PARM, &parm);
    // struct v4l2_control ctrl;
    // ioctl(cameraFd, VIDIOC_S_CTRL, &ctrl);

    // 申请缓冲区
    struct v4l2_requestbuffers req;
    req.count = BUFFCOUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    iret = ioctl(cameraFd, VIDIOC_REQBUFS, &req);
    if(iret < 0) {
        perror("request buffer error");
        // return;
    }

    void *addr[BUFFCOUNT];
    int size[BUFFCOUNT];
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    for(int i=0; i<BUFFCOUNT; i++) {
        buf.index = i;
        iret = ioctl(cameraFd, VIDIOC_QUERYBUF, &buf);
        if(iret < 0) {
            perror("query buffer error");
            // return;
        }
        // 映射缓冲区
        addr[buf.index] = mmap(NULL, buf.length, \
                        PROT_READ|PROT_WRITE, MAP_SHARED, \
                        cameraFd, buf.m.offset);
        if(addr[i] == (void*)-1) {
            perror("mmap error");
            // return;
        }
        size[i] = buf.length;
        // 申请到的帧缓冲放入采集输出队列
        iret = ioctl(cameraFd, VIDIOC_QBUF, &buf);
        if(iret < 0) {
            perror("queue buffer error");
            // return;
        }
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int fd = open("./picture.yuv", O_RDWR|O_CREAT, 0600);//|O_APPEND
    if(fd<0) {
        perror("Open file");
    }

    // 开始采集
    iret = ioctl(cameraFd, VIDIOC_STREAMON, &type);
    if(iret < 0) {
        perror("start stream error");
        // return;
    }
    printf("Begin capture\n");

    // while(1) {
        iret = ioctl(cameraFd, VIDIOC_DQBUF, &buf);
        if(iret < 0) {
            perror("dequeue buffer error");
            // return;
        }
        printf("dqbuf\n");

        // graph process
        void *cur_addr = addr[buf.index];
        write(fd, cur_addr, buf.length);
        printf("write\n");

        unsigned char *jpg_p=malloc(WIDTH*HEIGHT*3);
        char jpg_file_name[100]; /*存放JPG图片名称*/
        int jpg_cnt=0;
        FILE *jpg_file;
        int jpg_size;
        /*YUV数据转JPEG格式*/
        jpg_size=yuv_to_jpeg(WIDTH,HEIGHT,WIDTH*HEIGHT*3,cur_addr,jpg_p,80);
        sprintf(jpg_file_name,"%d.jpg",jpg_cnt++);
        printf("图片名称:%s,字节大小:%d\n",jpg_file_name,jpg_size);
        jpg_file=fopen(jpg_file_name,"wb");
        fwrite(jpg_p,1,jpg_size,jpg_file);
        fclose(jpg_file);


        iret = ioctl(cameraFd, VIDIOC_QBUF, &buf);
        if(iret < 0) {
            perror("queue buffer error");
            // return;
        }
        printf("qbuf\n");
    // }

    ioctl(cameraFd, VIDIOC_STREAMOFF, &type);
    printf("Stop capture\n");

    for(int i=0; i<BUFFCOUNT; i++) {
        munmap(addr[i], size[i]);
    }
    close(fd);
    close(cameraFd);
    return 0;
}

