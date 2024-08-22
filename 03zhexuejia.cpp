#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <vector>
#include <unistd.h>

std::vector<sem_t> mutex(5);
sem_t count;

void philosopher(int id) {
    while (true) {
        sem_wait(&count);  // 尝试进入餐厅

        std::cout << "哲学家【" << id << "】进场了!" << std::endl;

        sem_wait(&mutex[id]);  // 拿起左边的筷子
        std::cout << "哲学家【" << id << "】拿到了筷子：" << id << "！" << std::endl;

        sem_wait(&mutex[(id + 1) % 5]);  // 拿起右边的筷子
        std::cout << "哲学家【" << id << "】拿到了筷子：" << (id + 1) % 5 << "！" << std::endl;

        std::cout << "哲学家【" << id << "】号吃了通心粉！" << std::endl;

        sleep(5);  // 吃饭时间

        // 放下筷子
        std::cout << "哲学家【" << id << "】号放下了筷子：" << id << "," << (id + 1) % 5 << "！" << std::endl;
        sem_post(&mutex[id]);
        sem_post(&mutex[(id + 1) % 5]);

        sem_post(&count);  // 离开餐厅
        std::cout << "哲学家【" << id << "】离场了!" << std::endl;
    }
}

int main() {
    // 初始化信号量
    sem_init(&count, 0, 4);  // 最多4个哲学家可以同时进场
    for (int i = 0; i < 5; i++) {
        sem_init(&mutex[i], 0, 1);  // 每个筷子初始化为1（即可用）
    }

    // 创建哲学家线程
    std::thread philosophers[5];
    for (int i = 0; i < 5; i++) {
        philosophers[i] = std::thread(philosopher, i);
    }

    // 等待所有哲学家线程结束
    for (int i = 0; i < 5; i++) {
        philosophers[i].join();
    }

    // 销毁信号量
    sem_destroy(&count);
    for (int i = 0; i < 5; i++) {
        sem_destroy(&mutex[i]);
    }

    return 0;
}
