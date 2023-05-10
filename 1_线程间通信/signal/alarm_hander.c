#include <stdio.h> 
#include <stdlib.h> 
#include <signal.h> 
#include <unistd.h> 
 
//  #include <signum.h>

static void sig_handler(int sig) 
{
    static int tim = 0;
    tim++;
    printf("Alarm timeout : %d  \n", tim); 
    // exit(0); 
    
    sleep(10);
    alarm(2);           // 两秒执行一次 handler

    // 或创建一个线程。。   sleep(2); /
} 
 

static void sig2_handler(int sig) 
{
    static int tim = 0;
    tim++;
    printf("Alarm timeout : %d  \n", tim); 


    // shell:  kill -SIGALRM PID            #向PID发送SIGALRM信号，直接执行中断程序。 
    // shell:  killall -s SIGALRM pro_name
} 



int main(int argc, char *argv[]) 

{ 
struct sigaction sig = {0}; 
int second; 

/* 检验传参个数 */ 
if (2 > argc) 
    exit(-1); 

/* 为 SIGALRM 信号绑定处理函数 */ 
sig.sa_handler = sig2_handler; 
sig.sa_flags = 0; 
if (-1 == sigaction(SIGALRM, &sig, NULL)) { 
    perror("sigaction error"); 
    exit(-1); 
}

/* 启动 alarm 定时器 */ 
second = atoi(argv[1]); 
printf("定时时长: %d 秒\n", second); 
alarm(second); 

/* 循环 */ 
for ( ; ; ) {
    sleep(10); 
    // alarm(second); 
    printf("定时时长: %d 秒\n", second); 
}
    

exit(0); 

}