#include <iostream>
#include <pthread.h>
#include <unistd.h>

//线程执行函数
void* threadFunction(void* arg)
{
    pthread_t threadID = pthread_self();
    std::cout << "Thread ID: " << threadID << "executing." << std::endl;
    int* num = static_cast<int*>(arg);//将参数void*强制转换为int*
    std::cout << "Argument passed to thread: " << *num << std::endl;
    pthread_exit((void*)num);//线程退出，并将num作为返回值。
    
}




int main(){
    pthread_t thread;  //定义线程标识符变量，用来存储线程id
    pthread_attr_t attr;//定义线程属性对象，用于设置线程属性
    int arg = 42;//线程执行函数的参数
    void* retval;//定义一个指针变量，用来接受线程的返回值

    pthread_attr_init(&attr);//初始化线程属性
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);//线程属性设置分离状态，使得线程结束自己回收资源

    size_t stackSize = 1024 * 1024;//1MB 设置线程栈大小
    pthread_attr_setstacksize(&attr, stackSize);//线程属性设置栈大小

    if(pthread_create(&thread,&attr,threadFunction,&arg) != 0)
    {
        std::cerr << "Error creating thread!" << std::endl;
        return 1;
    }

    pthread_attr_destroy(&attr);//销毁线程属性对象，释放分配的资源
    sleep(1);//主线程休眠，保证子线程完成执行
    std::cout << "Main thread exiting" << std::endl;
    return 0;//主线程结束 

    //


}