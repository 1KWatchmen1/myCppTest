#include <iostream>
#include <unistd.h>// 包含 UNIX 标准函数库，用于 fork()、sleep() 等
#include <sys/wait.h>//// 包含等待子进程的函数库
#include <cstdio>
//此程序实现的是三个进程都打印1-100
void printNumber(const char* processName)
{
    for(int i = 1; i <= 100; i++)
    {
        printf("%s prints: %d\n", processName, i);
        usleep(100000);//0.1秒的延迟，让其他进程有机会运行
    }
}

int main()
{
    pid_t pid1, pid2;//用来保存进程标号

    /*fork() 函数：fork() 用于创建子进程。调用一次，返回两次：
    在父进程中返回子进程的PID，而在子进程中返回0。*/
    pid1 = fork();//创建子进程1
    if(pid1 < 0)//创建失败
    {
        std::cerr << "Fork failed!" << std::endl;
        return 1;
    }

    if(pid1 == 0)//// 如果 pid1 等于0，说明当前进程是第一个子进程
    {
        printNumber("Process 1");
        return 0;
    }
    else{// 如果 pid1 不等于0，说明当前进程是父进程
        pid2 = fork();//创建子进程2
        if(pid2 < 0)//创建失败
        {
            std::cerr << "Fork failed" << std::endl;
            return 1;
        }
        if(pid2 == 0)//子进程2
        {
            printNumber("Process 2");
            return 0;
        }
        else{   //主进程
            printNumber("Parent Process");
        }
        waitpid(pid1, nullptr,0);//等待子进程结束并回收资源，防止出现僵尸进程。
        waitpid(pid2, nullptr, 0);
    }
    std::cout << "All processes finished." << std::endl;
    return 0;// 父进程结束，返回0表示程序正常退出
}