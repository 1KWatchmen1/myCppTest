#include <iostream>
#include <mutex>
#include <atomic>

class Singleton3{
public:
//静态返回单例的成员函数：确保只返回一个，下方是懒汉模式；
    static Singleton3* GetInstance()
    {
        //当 lock_guard 对象超出作用域时（通常是在作用域结束时），它的析构函数会自动调用，从而释放 _mutex 上的锁。
        std::lock_guard<std::mutex> lock(_mutex);//将 _mutex 传递给 lock_guard 构造函数
        if(_instance == nullptr)
        {
            _instance = new Singleton3();
            //atexit 是一个标准库函数，它接受一个 void(*)() 类型的函数指针，在程序正常终止时调用它。你可以将 Destructor 函数传递给 atexit 来确保在程序结束时释放单例对象。
            atexit(Destructor);//atexit接口，在程序退出的时候，调用func:Destructor函数来释放堆上的资源；我们需要实现这个函数
        }
        return _instance;
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
        if(nullptr != _instance)
        {
            delete _instance;
            _instance = nullptr;
        }
    }

    //私有构造函数和析构函数
    Singleton3(){}
    ~Singleton3()
    {
        std::cout << "~Singleton3()\n" ;
    }
    //禁用四个构造韩素华
    Singleton3(const Singleton3 &) = delete;//拷贝构造函数
    Singleton3& operator=(const Singleton3&) = delete;//拷贝赋值构造函数
    Singleton3(Singleton3 &&) = delete;//移动拷贝构造函数
    Singleton3& operator=(Singleton3 &&) = delete;//移动拷贝赋值构造函数
    //静态成员变量
    static Singleton3* _instance;
    static std::mutex _mutex;
};
Singleton3* Singleton3::_instance = nullptr;
std::mutex Singleton3::_mutex; 

int main()
{
    Singleton3 *s1 = Singleton3::GetInstance();
    return 0;
}