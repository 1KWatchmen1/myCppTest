#include <iostream>
#include <mutex>
#include <atomic>

class Singleton2{
public:
//静态返回单例的成员函数：确保只返回一个，下方是懒汉模式；
    static Singleton2* GetInstance()
    {   //可见性：能够得到最新结果
        Singleton2* tmp = _instance.load(std::memory_order_relaxed);//使用 std::memory_order_relaxed 来存储和加载 _instance，这意味着在多线程环境中，这些操作不会影响其他线程对内存的可见性。即，操作只在本线程中是顺序的。
        //内存屏障：下面的代码不能优化到上面去，这是因，是一个获得操作:确保在此屏障之前的所有读操作在此屏障之后的读操作之前完成。
        std::atomic_thread_fence(std::memory_order_acquire);//std::atomic_thread_fence(std::memory_order_acquire) 用于在读取 _instance 之前防止重排序，使得在此之前的所有加载操作对本线程可见。
        if(tmp == nullptr)
        {   
            tmp = new Singleton2;
            //内存屏障：这是一个释放操作，这是一个果；内存屏障的作用就是保证内存优化不能够外溢到这两条线之外；:确保在此屏障之后的所有写操作在此屏障之前的写操作之后完成。
            std::atomic_thread_fence(std::memory_order_release);//std::atomic_thread_fence(std::memory_order_release) 用于在存储 _instance 之后防止重排序，确保之前的所有存储操作对其他线程可见。
            //xiedao1
            _instance.store(tmp,std::memory_order_relaxed);//_instance.store(tmp, std::memory_order_relaxed); 是一个原子操作，用于将 tmp 存储到 _instance 变量中。
            //atexit 是一个标准库函数，它接受一个 void(*)() 类型的函数指针，在程序正常终止时调用它。你可以将 Destructor 函数传递给 atexit 来确保在程序结束时释放单例对象。
            atexit(Destructor);//atexit接口，在程序退出的时候，调用func:Destructor函数来释放堆上的资源；我们需要实现这个函数
        }
        return tmp;
    }

private:
/*执行流程：
        1、当 atexit(Destructor) 被调用时，Destructor 函数会被注册为程序终止时要调用的函数。
        2、程序在正常结束时，C++ 运行时会依次调用通过 atexit 注册的所有函数。
        3、Destructor 函数内部通过 delete _instance 删除单例对象 _instance。
        4、当 delete _instance 被执行时，Singleton2 类的析构函数会被调用。
        Destructor 函数不会自动调用析构函数，而是通过 delete _instance 来触发析构函数（在 C++ 中，当你 delete 一个对象指针时，该对象的析构函数会被调用，用于释放资源并执行清理工作。）
        */
    static void Destructor()
    {
        Singleton2* tmp = _instance.load(std::memory_order_relaxed);
        if(nullptr != tmp)
        {
            delete tmp;
        }
    }

    //私有构造函数和析构函数
    Singleton2(){}
    ~Singleton2()
    {
        std::cout << "~Singleton2()\n" ;
    }
    //禁用四个构造韩素华
    Singleton2(const Singleton2 &) = delete;//拷贝构造函数
    Singleton2& operator=(const Singleton2&) = delete;//拷贝赋值构造函数
    Singleton2(Singleton2 &&) = delete;//移动拷贝构造函数
    Singleton2& operator=(Singleton2 &&) = delete;//移动拷贝赋值构造函数
    //原子变量
    static std::atomic<Singleton2*> _instance;
    static std::mutex _mutex;
};
std::atomic<Singleton2*> Singleton2::_instance;
std::mutex Singleton2::_mutex;

int main()
{
    Singleton2 *s1 = Singleton2::GetInstance();
    return 0;
}