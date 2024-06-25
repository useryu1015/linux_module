/**
 * @file serial_rxtx_test.c
 * @brief 
 * @make：
 *      gcc serial_rxtx_test.c -lrt -lpthread -o serial_rxtx_test 
 * @version 0.1
 * @date 2024-05-30
 * 
 * @copyright Copyright (c) 2024
 * @todo
 *  1. 16进制收发/ascii收发 模式可选
 */
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

union _format_crc{
	unsigned short crc;	
	unsigned char data[2];
} format_crc;

/***************************    CRC    ******************************/
/* Table of CRC values for high-order byte */
static const uint8_t table_crc_hi[] = {                                                                   //
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80,                                     //
    0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, //
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, //
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, //
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, //
    0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, //
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, //
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, //
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, //
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, //
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, //
    0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, //
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, //
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, //
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, //
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40};

/* Table of CRC values for low-order byte */
static const uint8_t table_crc_lo[] = {                                                                   //
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07,                                     //
    0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, //
    0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, //
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, //
    0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, //
    0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, //
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, //
    0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, //
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, //
    0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, //
    0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 0x77, //
    0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, //
    0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, //
    0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, //
    0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, //
    0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40};

static uint16_t crc16(uint8_t *buffer, uint16_t buffer_length)			// 查表法
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (buffer_length--) {
        i = crc_hi ^ *buffer++; /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}

static speed_t get_serial_speed(int baud)
{
    switch (baud)
    {
    case 110:
        return B110;
    case 300:
        return B300;
    case 600:
        return B600;
    case 1200:
        return B1200;
    case 2400:
        return B2400;
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    default:
        return B9600;
    }
}

static int open_serial(char *identifier, int baud, int data_bits, int stop_bits, char parity)
{
    int fd;
    struct termios tios;
    speed_t speed;
    char devname[40];

    printf("open serial identifier:%s baud:%d data_bits:%d stop_bits:%d parity:%c\n", identifier, baud, data_bits, stop_bits, parity);
    sprintf(devname, "%s", identifier);
    fd = open(devname, O_RDWR | O_NOCTTY | O_NDELAY | O_EXCL);
    if (fd <= 0)
    {
        printf("Can't open the device %s\n", devname);
        return -1;
    }

    memset(&tios, 0, sizeof(struct termios));

    /* Set the baud rate */
    speed = get_serial_speed(baud);
    if ((cfsetispeed(&tios, speed) < 0) || (cfsetospeed(&tios, speed) < 0))
    {
        close(fd);
        return -1;
    }

    tios.c_cflag |= (CREAD | CLOCAL);
    tios.c_cflag &= ~CSIZE;
    switch (data_bits)
    {
    case 5:
        tios.c_cflag |= CS5;
        break;
    case 6:
        tios.c_cflag |= CS6;
        break;
    case 7:
        tios.c_cflag |= CS7;
        break;
    case 8:
    default:
        tios.c_cflag |= CS8;
        break;
    }

    switch (stop_bits)
    {
    case 1:
        tios.c_cflag &= ~CSTOPB;
        break;
    default:
        tios.c_cflag |= CSTOPB;
        break;
    }

    switch (parity)
    {
    case 'E':
        tios.c_cflag |= PARENB;
        tios.c_cflag &= ~PARODD;
        break;
    case 'O':
        tios.c_cflag |= PARENB;
        tios.c_cflag |= PARODD;
        break;
    case 'N':
    default:
        tios.c_cflag &= ~PARENB;
    }

    // Software flow control is disabled
    tios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // INPCK        Enable parity check
    switch (parity)
    {
    case 'N':
        tios.c_iflag &= ~INPCK;
        break;
    default:
        tios.c_iflag |= INPCK;
        break;
    }

    tios.c_iflag &= ~(IXON | IXOFF | IXANY);

    // OPOST        Postprocess output (not set = raw output)
    tios.c_oflag &= ~OPOST;

    // VMIN         Minimum number of characters to read
    tios.c_cc[VMIN] = 0;
    // VTIME        Time to wait for data (tenths of seconds)
    tios.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tios) < 0)
    {
        close(fd);
        return -1;
    }

    return fd;
}

/***************************    OPT    ******************************/
static int optval, baud = 115200;
static int data_bits = 8;
static int stop_bits = 1;
static int mode = 0;
static char parity = 'N';
static char dev[256] = {"/dev/ttyS0"};

void Usage(char *cmd)
{
    printf("\nUsage: %s [OPTION]...\n", cmd);
    printf("\t-t Device Identifier [default /dev/ttyS0]\n");
    printf("\t-b Baud int [default 115200]\n");
    printf("\t-d Data Bits int [default 8]\n");
    printf("\t-p Parity  char [default N]\n");
    printf("\t-s Stop Bits int [default 1]\n");
    printf("\t-m 1: Use CRC16, 0: No CRC16 [default 0]\n");
    printf("\t-h Show this message\n\n");
    exit(0);
}

void opt_parse(int argc, char *argv[])
{
    while ((optval = getopt(argc, argv, ":t:b:d:p:s:h:m:")) != -1)
    {
        switch (optval)
        {
        case 't':
            snprintf(dev, sizeof(dev), "%s", optarg);
            break;
        case 'b':
            sscanf(optarg, "%d", &baud);
            break;
        case 'd':
            sscanf(optarg, "%d", &data_bits);
            break;
        case 'p':
            parity = *optarg;
            break;
        case 's':
            sscanf(optarg, "%d", &stop_bits);
            break;
        case 'm':
            sscanf(optarg, "%d", &mode);
            break;
        default:
            Usage(argv[0]);
            break;
        }
    }
}

/***************************    Main API    ******************************/
static int running = true;
static int g_serial_fd = -1;


/*
 * change "E60701" to hex: 0xE60701
 */
int string_to_hex(char *buf, char *val)
{
	int i = 0;
	int num;
	
	// printf(" datalen: %d \n", strlen(buf)/2);
	// printf(" str buf: %s \n", buf);
	
	for(; i<strlen(buf)/2; i++){
		sscanf(buf+i*2, "%2x", val+i);
		// printf(" %02X", *((char*)(val+i)));
	}
	// printf("\n");

	return strlen(buf)/2;
}

void cut_space(char* Res, char* Dec)
{
	//int i = 0;
	for (; *Res != '\0'; Res++) {
		if (*Res != ' ')
			*Dec++ = *Res;//'++' 优先级比 '*'高，但是这里'++'作为后缀，先进行*Dec，再自加
	}

	*Dec = '\0';
	return;
}

static void printf_array(char *str, uint8_t *buffer, uint32_t sz)
{
    uint32_t i = 0;

    if (!buffer)
        return;

    if (str)
        fprintf(stdout, "%s", str);

    for (i = 0; i < sz; ++i)
    {
        fprintf(stdout, "%02X", buffer[i]);
    }
    fprintf(stdout, "\n");
    fflush(stdout);
}

static int serial_txd(int fd, void *ptr, int size)
{
    int ret;
    char *ps, *pe;
    ps = (char *)ptr;
    pe = (char *)ptr + size;
    while (pe > ps)
    {
        ret = write(fd, ps, pe - ps);
        if (ret < 0)
            return -1;

        ps += ret;
    }

    printf_array("\033[32mTX:\033[0m", ptr, (ps - (char *)ptr));
    return ps - (char *)ptr;
}

static int serial_rxd(int fd, char *ptr, int size, int timeout)
{
    time_t t;
    int ret, s_rc;
    
    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = 1000 * timeout;

    s_rc = select(fd + 1, &rfds, NULL, NULL, &tv);
    if (s_rc > 0)
    {
        ret = read(fd, ptr, size);
        if (ret > 0)
        {
            time(&t);
            printf("\n");
            printf("\033[31m%s\033[0m", ctime(&t));
            printf_array("\033[31mRX:\033[0m", ptr, ret);
            // printf("string: %s", ptr);
            // memset(ptr,0,8);
        }
    }

    return ret;
}

void *run_serial_rxd_thread(void *args)
{
    char rbuf[65536];

    while (running)
    {
        serial_rxd(g_serial_fd, rbuf, sizeof(rbuf), 500);
    }
}


int main(int argc, char *argv[])
{
    pthread_t tid;
    uint8_t sbuf[2048] = {0};
    uint8_t input[2048] = {0};
    int b, i, ret, slen = sizeof(sbuf);

    opt_parse(argc, argv);

    /* init serial */
    g_serial_fd = open_serial(dev, baud, data_bits, stop_bits, parity);
    if (g_serial_fd <= 0)
    {
        printf("open serial faild\n");
        exit(0);
    }

    /* init serial recv thread */
    tcflush(g_serial_fd, TCIOFLUSH);
    pthread_create(&tid, NULL, run_serial_rxd_thread, NULL);


    /* main loop */
    printf("\nEnter message to be send: \n");
	while (input[0] != 'q')
	{
        usleep(300 *1000);

        // printf("\nEnter message to be send (q = quit): ");
        if (fgets(input, sizeof(input), stdin) != NULL) {
            // printf("STR:%s", input);
            cut_space(input, sbuf);

            slen = string_to_hex(sbuf, input);

            /* 添加校验位 */
            if (mode == 1) {
		        uint16_t crc_buf = crc16((uint8_t*)input, slen);		// 计算校验值

                format_crc.crc = crc_buf;
                printf("crc is: %.2X %.2X \n", format_crc.data[0], format_crc.data[1]);

                uint8_t *ps = input + slen;
                *(ps++) = format_crc.data[1];
                *(ps++) = format_crc.data[0];
                *ps = '\0';
                slen += 2;
            }

            /* 打印发送报文 */
	        for (i = 0; i < slen; i++)
	            printf("<%.2X>", ((uint8_t*)input)[i]);
            printf(" to be sent.\n");

            serial_txd(g_serial_fd, input, slen);
        }
    }

    pthread_join(tid, NULL);
    close(g_serial_fd);
    return 0;
}