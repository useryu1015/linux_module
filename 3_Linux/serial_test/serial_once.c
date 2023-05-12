#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>
#define BAUDRATE        B115200
#define UART_DEVICE     "/dev/ttyS5"

#define FALSE  -1
#define TRUE   0
///////////////////////////////////////////////////////////////////
/**
*@brief  设置串口通信速率
*@param  fd     类型 int  打开串口的文件句柄
*@param  speed  类型 int  串口速度
*@return  void
*/
int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300,
                   B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {115200, 38400, 19200, 9600, 4800, 2400, 1200,  300, 
                  115200, 38400, 19200, 9600, 4800, 2400, 1200,  300, };
void set_speed(int fd, int speed){
  int   i; 
  int   status; 
  struct termios   Opt;
  tcgetattr(fd, &Opt); 
  for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++) {
    if  (speed == name_arr[i]) {
      tcflush(fd, TCIOFLUSH);
      cfsetispeed(&Opt, speed_arr[i]);
      cfsetospeed(&Opt, speed_arr[i]);
      status = tcsetattr(fd, TCSANOW, &Opt);
      if  (status != 0) {
        perror("tcsetattr fd1");
        return;
      }    
      tcflush(fd,TCIOFLUSH);
    }
  }
}
///////////////////////////////////////////////////////////////////
/**
*@brief   设置串口数据位，停止位和效验位
*@param  fd     类型  int  打开的串口文件句柄
*@param  databits 类型  int 数据位   取值 为 7 或者8
*@param  stopbits 类型  int 停止位   取值为 1 或者2
*@param  parity  类型  int  效验类型 取值为N,E,O,,S
*/
int set_Parity(int fd,int databits,int stopbits,int parity)
{ 
    struct termios options; 
    if  ( tcgetattr( fd,&options)  !=  0) { 
        perror("SetupSerial 1");     
        return(FALSE);  
    }
    options.c_cflag &= ~CSIZE; 
    switch (databits) /*设置数据位数*/
    {   
    case 7:     
        options.c_cflag |= CS7; 
        break;
    case 8:     
        options.c_cflag |= CS8;
        break;   
    default:    
        fprintf(stderr,"Unsupported data size\n"); return (FALSE);  
    }
    switch (parity) 
    {   
        case 'n':
        case 'N':    
            options.c_cflag &= ~PARENB;   /* Clear parity enable */
            options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
            break;  
        case 'o':   
        case 'O':     
            options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/  
            options.c_iflag |= INPCK;             /* Disnable parity checking */ 
            break;  
        case 'e':  
        case 'E':   
            options.c_cflag |= PARENB;     /* Enable parity */    
            options.c_cflag &= ~PARODD;   /* 转换为偶效验*/     
            options.c_iflag |= INPCK;       /* Disnable parity checking */
            break;
        case 'S': 
        case 's':  /*as no parity*/   
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;break;  
        default:   
            fprintf(stderr,"Unsupported parity\n");    
            return (FALSE);  
        }  
    /* 设置停止位*/  
    switch (stopbits)
    {   
        case 1:    
            options.c_cflag &= ~CSTOPB;  
            break;  
        case 2:    
            options.c_cflag |= CSTOPB;  
           break;
        default:    
             fprintf(stderr,"Unsupported stop bits\n");  
             return (FALSE); 
    } 
    /* Set input parity option */ 
    if (parity != 'n')   
        options.c_iflag |= INPCK; 
    tcflush(fd,TCIFLUSH);
    options.c_cc[VTIME] = 150; /* 设置超时15 seconds*/   
    options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
    if (tcsetattr(fd,TCSANOW,&options) != 0)   
    { 
        perror("SetupSerial 3");   
        return (FALSE);  
    } 
    options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    options.c_oflag  &= ~OPOST;   /*Output*/
    return (TRUE);  
}


int pesam_write(int fd, const uint8_t* writebuf, int len){

    int writelen = write(fd, writebuf, len);   // 向PESAM发送获取芯片信息命令数据
    if (writelen < 0)
    {
    perror("write\n");
    close(fd);
    exit(1);
    return -1;
    }
    return writelen;
}

int pesam_read(int fd, uint8_t* readbuf, int len){
    
    int readlen = read(fd,readbuf,len);    // 从PESAM接收命令响应数据
    if (readlen < 0)
    {
        perror("read\n");
        close(fd);
        exit(1);
        return -1;
    }
    return readlen;
}

/*
 * 将字符串: "E60701" to hex: 0xE60701
 * tips：1. 如果平台给的是十进制数的字符串？？
 *       2. val的长度可变？？
 */
void string2hex(char *buf, uint8_t *val)
{
	int i = 0;
	int num;

	// memset();
	// printf(" json DataLen: %d \n", strlen(buf)/2);
	// printf(" buf: %s  %d\n", buf, sizeof(val));
	
	for(; i<strlen(buf)/2 /*&& i<sizeof(val)*/; i++){               // BUG：2022年6月8日  sizo(val) 为4， 而非1024      use二级指针？
		sscanf(buf+i*2, "%2x", val+i);
		// printf("    --- %x \n", *(val+i));
	}

}

void zlog_info(char *tips, uint8_t *buf, int len){
    int i=0;
    
    printf("%s[%d]: 0x", tips, len);
    for (; i<len; i++)
        printf("%02x ", (uint8_t)buf[i]);
    
    printf("\n");
}


// 该select仅检测单个fd
static int serial_select(int fd, long milliseconds, int need_size)
{
	int s_rc;
	fd_set rfds;
	struct timeval tv;

	/* Add a file descriptor to the set */
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);

	/* set timeout */
	// tv.tv_sec = 0;
	// tv.tv_usec = milliseconds * 1000;

	// printf(" fd: %d \n", fd);
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	errno = 0;				// 2022Äê10ÔÂ10ÈÕ£º ³õÊ¼»¯ÎªÁã£¬²âÊÔ
	while ((s_rc = select(fd + 1, &rfds, NULL, NULL, &tv)) == -1) {
		if (errno == EINTR) {
			printf("    A non blocked signal was caught\n");		    // ÔÙ´Î·¢Æð¶ÁÈ¡ÇëÇó
			/* Necessary after an error */
			FD_ZERO(&rfds);
			FD_SET(fd, &rfds); 
		} else {
			printf("    select error!\n");
			// printf("%s: select", strerror(errno));
			return -1;
		}
	}

	if (s_rc == 0) {
		/* Timeout */
		errno = ETIMEDOUT;
		return -1;
	}
	
	return s_rc;
}

///////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    int fd,i;
    char *dev = argv[1];

    int writelen, readlen;
    uint8_t sbuf[256] = {0x11,0x55,0xff};
    uint8_t rbuf[256];

    printf("Start...\n");

    for (i=1; i<argc; i++)
    {
        dev = argv[i];

        /******** 初始化串口配置 **********/
        printf("    open: %s \n", dev);
        fd = open(dev, O_RDWR);
        if (fd < 0)
        {
            perror(UART_DEVICE);
            exit(1);
        }
        set_speed(fd,115200);
        if (set_Parity(fd,8,1,'N') == FALSE)  
        {
            printf("    Set Parity Error\n");
            exit (0);
        }
        /******** end **********/


// 刘淼 10-24 10:55:39
// 设置频点：2450415241000F5930A92E0000FF014501014011FB16
// 刘淼 10-24 10:55:48
// 返回：2450415241000D5870A92E0000FF010545AA4E06

        string2hex("2450415241000F5930A92E0000FF014501014011FB16", sbuf);   // 设置频点为0x11
        string2hex("2450415241000F5930A92E0000FF0145010140460557", sbuf);   // 设置频点为0x11

        // string2hex("2450415241000F5930A92E0000FF0145010140193D17", sbuf);   // 设置频点为25
        // CRC_CHECK(rbuf, 6);
        tcflush(fd, TCIOFLUSH);

        // zlog_info("    -------write", sbuf, 22);
        // writelen = pesam_write(fd, sbuf, 22);

        // printf("    send:%d begin read \n", writelen);

        while(1) {
            // if (serial_select(fd, 0, 0) < 0) {
            //     // if (errno == ETIMEDOUT) 
            //     //     printf("    recv done \n");
            //     // else
            //     //     printf("    select errno \n");
            //     // break;      // 超时后退出 read while
            // }
            readlen = pesam_read(fd,rbuf,256);
            zlog_info("    -------read", rbuf, readlen);
        }

        printf("Close...\n");        
        close(fd);
    }


    return 0;
}

