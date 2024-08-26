#include <iostream>
#include <mutex>
#include <atomic>

class Singleton1{
public:
//静态返回单例的成员函数：确保只返回一个，下方是懒汉模式；
    static Singleton1* GetInstance()
    {
        if(_instance == nullptr)
        {
            _instance = new Singleton1();
        }
        return _instance;
    }

private:
    //私有构造函数和析构函数
    Singleton1(){}
    ~Singleton1()
    {
        std::cout << "~Singleton1()\n" ;
    }
    //禁用四个构造韩素华
    Singleton1(const Singleton1 &) = delete;//拷贝构造函数
    Singleton1& operator=(const Singleton1&) = delete;//拷贝赋值构造函数
    Singleton1(Singleton1 &&) = delete;//移动拷贝构造函数
    Singleton1& operator=(Singleton1 &&) = delete;//移动拷贝赋值构造函数
    //静态成员变量
    static Singleton1* _instance;
};
Singleton1* Singleton1::_instance = nullptr;

int main()
{
    Singleton1 *s1 = Singleton1::GetInstance();
    return 0;
}