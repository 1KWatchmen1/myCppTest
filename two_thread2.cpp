#include <iostream>
#include <thread>
#include <condition_variable>

std::condition_variable cv;
std::mutex mtx;
int counter = 1;
bool turn = true;  // 用于表示轮到哪个线程打印

void print_odd() {
    while (counter <= 100) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return turn; });  // 等待轮到这个线程
        if (counter <= 100 && counter % 2 == 1) {
            std::cout << "Thread 1: " << counter << std::endl;
            ++counter;
        }
        turn = false;  // 轮到另一个线程
        cv.notify_all();  // 唤醒其他线程
    }
}

void print_even() {
    while (counter <= 100) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !turn; });  // 等待轮到这个线程
        if (counter <= 100 && counter % 2 == 0) {
            std::cout << "Thread 2: " << counter << std::endl;
            ++counter;
        }
        turn = true;  // 轮到另一个线程
        cv.notify_all();  // 唤醒其他线程
    }
}

int main() {
    std::thread t1(print_odd);
    std::thread t2(print_even);

    t1.join();
    t2.join();

    return 0;
}

