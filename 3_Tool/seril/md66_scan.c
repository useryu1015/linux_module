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

static int running = true;

/*
 * 表计功能码0x66规约的固定头部*/
typedef struct _md66_head_t
{
    uint8_t addr; // 地址域
    uint8_t ex66; // modbus扩展固定功能码
    uint8_t len;  // 数据帧长度
} __attribute__((packed)) md66_head_t;

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
uint16_t crc16(uint8_t *buffer, uint16_t buffer_length)
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i;        /* will index into CRC lookup */

    /* pass through message buffer */
    while (buffer_length--)
    {
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

    printf("open_serial identifier:%s baud:%d data_bits:%d stop_bits:%d parity:%c\n", identifier, baud, data_bits, stop_bits, parity);

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

static int serial_send(int fd, const void *ptr, int size)
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
    // printf_array("serial_send:", ptr, (ps - (char *)ptr));
    return ps - (char *)ptr;
}

static int serial_recv(int fd, void *ptr, int size, int slave)
{
    int s_rc, ret, head = false;
    char *ps = ptr;
    md66_head_t *phead = ptr;

    if (size < sizeof(md66_head_t))
    {
        return -1;
    }

    char *pe = ps + sizeof(md66_head_t);
    while (ps < pe)
    {
        fd_set rfds;
        struct timeval tv;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        tv.tv_sec = 0;
        tv.tv_usec = 1000 * 200;

        s_rc = select(fd + 1, &rfds, NULL, NULL, &tv);
        if (s_rc > 0)
        {
            ret = read(fd, ps, pe - ps);
            if (ret <= 0)
            {
                return ret;
            }
            ps += ret;

            if (!head &&
                phead->addr == slave &&
                phead->ex66 == 0x66)
            {
                printf("slave_id %d is success\n", slave);
                head = true;
                pe = ps + phead->len + 2;
            }
        }
        else if (s_rc == 0)
        {
            // printf("id %d timeout\n",slave);
            return -2;
        }
        else
        {
            printf("select error\n");
            return -3;
        }
    }
    printf_array("serial_recv:", ptr, ps - (char *)ptr);
    return ret;
}

int str_to_hex(char *hexstr, char *ssal, int *len)
{
    int i;
    char temp[8] = {0};
    for (i = 0; (i < strlen(hexstr) / 2) && (i < *len); i++)
    {
        temp[0] = hexstr[i * 2];
        temp[1] = hexstr[i * 2 + 1];
        sscanf(temp, "%02x", &ssal[i]);
    }
    *len = i;
    return 0;
}

void Usage(char *cmd)
{
    printf("\nUsage: %s [OPTION]...\n", cmd);
    printf("\t-t Device Identifier [default /dev/ttyS0]\n");
    printf("\t-b Baud int [default 115200]\n");
    printf("\t-d Data Bits int [default 8]\n");
    printf("\t-p Parity  char [default N]\n");
    printf("\t-s Stop Bits int [default 1]\n");
    printf("\t-h Show this message\n\n");
    exit(0);
}

/*信号处理*/
static void signal_handler(int signo)
{
    running = false;
    return;
}

int main(int argc, char *argv[])
{
    char *ps = NULL, sbuf[2048] = {0}, rbuf[2048] = {0};
    int b, i, ret, slen = sizeof(sbuf);
    char dev[256] = {"/dev/ttyS0"};
    int fd, optval, baud = 115200;
    int data_bits = 8;
    int stop_bits = 1;
    uint16_t md66_oi = 2001;
    char parity = 'N';
    md66_head_t mhead;

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signal_handler);  /* ctrl^c */
    signal(SIGQUIT, signal_handler); /* ctrl^\ */
    signal(SIGTERM, signal_handler); /* kill/killall */

    while ((optval = getopt(argc, argv, ":t:b:d:p:s:h:o")) != -1)
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
        case 'o':
            sscanf(optarg, "%d", &md66_oi);
            break;
        default:
            Usage(argv[0]);
            break;
        }
    }
    int baud_arry[] = {4800, 9600, 19200, 38400, 57600, 115200};

    fd = open_serial(dev, baud, data_bits, stop_bits, parity);
    if (fd <= 0)
    {
        printf("open serial faild\n");
        exit(0);
    }

    for (i = 1; i < 255; i++)
    {
        if (!running)
        {
            break;
        }
        int s_rc;
        fd_set rfds;
        struct timeval tv;

        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        tv.tv_sec = 0;
        tv.tv_usec = 1000 * 200;

        ps = sbuf;
        sprintf(rbuf, "%02x6603012001", i);
        str_to_hex(rbuf, sbuf, &slen);
        ps += slen;
        *(uint16_t *)ps = htons(crc16(sbuf, ps - sbuf));
        ps += 2;
        serial_send(fd, sbuf, ps - sbuf);
        int rtn = serial_recv(fd, rbuf, sizeof(rbuf), i);
        if ( rtn > 0 )
            printf_array(" **** serial_recv:", rbuf, rtn);
    }
    close(fd);
    return 0;
}