#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>

sem_t sem1, sem2, sem3;

void printNumbers(const char* processName, int processId, sem_t* currentSem, sem_t* nextSem) {
    for (int i = processId; i <= 100; i += 3) {
        sem_wait(currentSem); // 等待当前信号量
        
        std::cout << processName << " prints: " << i << std::endl;
        usleep(100000); // 休眠0.1秒

        sem_post(nextSem); // 释放下一个信号量
    }
    sem_post(nextSem); // 确保最后一个进程不再等待
}

int main() {
    pid_t pid1, pid2;

    // 初始化信号量，sem1 初始值为1，其他信号量初始值为0
    sem_init(&sem1, 1, 1);
    sem_init(&sem2, 1, 0);
    sem_init(&sem3, 1, 0);

    pid1 = fork(); // 创建第一个子进程
    if (pid1 == 0) {
        printNumbers("Process 1", 2, &sem2, &sem3); // 子进程1打印
        return 0;
    } else {
        pid2 = fork(); // 创建第二个子进程
        if (pid2 == 0) {
            printNumbers("Process 2", 3, &sem3, &sem1); // 子进程2打印
            return 0;
        } else {
            printNumbers("Parent Process", 1, &sem1, &sem2); // 父进程打印

            // 等待子进程结束
            waitpid(pid1, nullptr, 0);
            waitpid(pid2, nullptr, 0);

            // 销毁信号量
            sem_destroy(&sem1);
            sem_destroy(&sem2);
            sem_destroy(&sem3);

            std::cout << "All processes finished." << std::endl;
            return 0;
        }
    }
}
