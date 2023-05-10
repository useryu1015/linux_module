#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#define SENDBUF     1024

typedef struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[SENDBUF];    /* message data */
}msgq_t;

const key_t g_msg_key = 1234;

int main(int argc, char* argv[])
{
    int msg_handle, ret, run_flag = 1;
    msgq_t data;
    char sendbuf[SENDBUF];
    long int msgtype = 1;

    msg_handle = msgget(g_msg_key, IPC_PRIVATE | IPC_CREAT);        // 获取消息队列的handle （类似fd）
    if (msg_handle == -1)
    {
        printf("msgget failed with error %d\n", errno);
        exit(EXIT_FAILURE);
    }

    while (run_flag)
    {
        printf("Please input msgtype and msg\n");
        scanf("%ld%s", &msgtype, sendbuf);

        data.mtype = msgtype;
        strcpy(data.mtext, sendbuf);

        // ret = msgsnd(msg_handle, &data, sizeof(msgq_t) - sizeof(long), 0);      // 向指定队列发送数据
        ret = msgsnd(msg_handle, &data, 1024, 0);
        if(ret == -1) {
            printf("msgsnd failed with error %d\n", errno);
            exit(EXIT_FAILURE);
        }

        if (strncmp(sendbuf, "end", 3) == 0)
        {
            run_flag = 0;
        }
    }
    
    ret = msgctl(msg_handle, IPC_RMID, 0);
    if(ret == -1) {
        printf("msgctl failed with error %d\n", errno);             // 删除数据， 也可以用于队列属性的获取和设置
        exit(EXIT_FAILURE);
    }

    return 0;
}





