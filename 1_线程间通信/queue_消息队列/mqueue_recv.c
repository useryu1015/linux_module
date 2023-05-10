#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

typedef struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[BUFSIZ];    /* message data */
}msgq_t;

const key_t g_msg_key = 1234;

int main(int argc, char* argv[])
{
    int msg_handle, ret;
    int run_flag = 1;
    long int msgtype = -5;
    msgq_t data;

    msg_handle = msgget(g_msg_key, IPC_PRIVATE | IPC_CREAT);
    if(msg_handle == -1) {
        printf("msgget failed with error %d\n", errno);
        exit(EXIT_FAILURE);
    }

    while (run_flag)
    {
        ret = msgrcv(msg_handle, &data, BUFSIZ, msgtype, 0);
        if(ret == -1) {
            printf("msgrcv failed with error %d\n", errno);
            exit(EXIT_FAILURE);
        } else {
            printf("recv queue type %ld, data: %s\n", data.mtext, data.mtext);
        }
        if(strncmp(data.mtext, "end", 3) == 0) {
            run_flag = 0;
        }
    }
    
    ret = msgctl(msg_handle, IPC_RMID, 0);
    if(ret == -1) {
        printf("msgctl failed with error %d\n", errno);
        exit(EXIT_FAILURE);    
    }

    return 0;
}
