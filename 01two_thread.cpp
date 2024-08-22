#include <iostream>
#include <pthread.h>

// 互斥锁和条件变量
pthread_mutex_t mtx;
pthread_cond_t cv;
bool turn = true;  // true: 线程1的回合, false: 线程2的回合

//打印函数，参数：指向一个布尔值的指针，标志当前线程是否是线程1。
void* print_numbers(void* is_thread_1_ptr) {
    bool is_thread_1 = *(bool*)is_thread_1_ptr;

    for (int i = is_thread_1 ? 1 : 2; i <= 100; i += 2) {
        pthread_mutex_lock(&mtx);

        // 等待条件满足
        //如果条件不满足（即turn的值与is_thread_1不匹配），线程将释放互斥锁并等待在条件变量cv上。
        while (turn != is_thread_1) {
            pthread_cond_wait(&cv, &mtx);//用于让当前线程等待条件变量cv，直到条件turn != is_thread_1不再满足。
        }

        // 打印当前数字
        std::cout << i << std::endl;

        // 轮到另一个线程
        turn = !turn;

        // 唤醒等待的线程
        pthread_cond_signal(&cv);//唤醒等待在条件变量cv上的另一个线程。
        pthread_mutex_unlock(&mtx);
    }

    return nullptr;
}

int main() {
    // 初始化互斥锁和条件变量
    pthread_mutex_init(&mtx, nullptr);
    pthread_cond_init(&cv, nullptr);

    // 创建线程  使用pthread_create创建两个线程t1和t2。
    pthread_t t1, t2;
    bool t1_is_thread_1 = true;
    bool t2_is_thread_1 = false;

    pthread_create(&t1, nullptr, print_numbers, &t1_is_thread_1);
    pthread_create(&t2, nullptr, print_numbers, &t2_is_thread_1);

    // 等待线程结束
    pthread_join(t1, nullptr);//用于阻塞主线程，直到t1和t2执行完毕。
    pthread_join(t2, nullptr);

    // 销毁互斥锁和条件变量
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cv);

    return 0;
}
/*&t1:这是一个指向 pthread_t 类型的指针，用于存储新创建的线程的标识符（ID）。在这里，t1 是主线程用于引用子线程的句柄。
nullptr:这个参数是线程的属性（pthread_attr_t），如果你不需要特定的属性，可以传递 nullptr，表示使用默认属性。
print_numbers:这是线程启动后将要执行的函数名称。在这个例子中，新创建的线程会开始执行 print_numbers 函数。
&t1_is_thread_1:这是传递给 print_numbers 函数的参数。在这里，将 t1_is_thread_1 的地址传递给新线程，这个布尔变量指示当前线程是否应该打印奇数。*/

//一旦调用 pthread_create，操作系统会启动一个新线程并开始执行 print_numbers 函数。新线程从这个函数开始执行，而主线程可以继续执行自己的任务。
//主线程调用 pthread_create，并将 print_numbers 函数作为新线程的入口点，传递 t1_is_thread_1 变量的地址作为参数。
