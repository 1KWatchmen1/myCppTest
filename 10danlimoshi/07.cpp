#include <iostream>
template<typename T>
class Singleton
{
public:
    //这里的T是子类，在基类中能调用子类私有的构造函数吗？：在子类中申请友元函数
    static T* GetInstance()//静态返回单例的成员函数
    {
        static T instance;//静态局部变量
        return &instance;
    }
protected://这里将构造函数和析构函数设置为了受保护的成员（为了区分外部和子类，对子类开放，对外部拒绝）
    Singleton(){std::cout << "Singleton()\n";}
    virtual ~Singleton(){
        std::cout << "~Singleton()\n";
    }
private:
    Singleton(const Singleton &) = delete;//拷贝构造函数
    Singleton& operator=(const Singleton&) = delete;//拷贝赋值构造函数
    Singleton(Singleton &&) = delete;//移动拷贝构造函数
    Singleton& operator=(Singleton &&) = delete;//移动拷贝赋值构造函数

};
//子类DesignPattern，作为参数传入模版类
class DesignPattern : public Singleton<DesignPattern>
{
    friend class Singleton<DesignPattern>;//友元函数，让父类可以访问本子类的私有成员
public:
///....
private:
    DesignPattern(){std::cout << "DesignPattern\n";}
    ~DesignPattern(){
        std::cout << "~DesignPattern()\n";
    }
};

int main()
{
    //当你调用 DesignPattern::GetInstance() 时，实际上是调用了 Singleton<DesignPattern>::GetInstance()。
    DesignPattern *s1 = DesignPattern::GetInstance();
    return 0;
}

//子类不会继承父类的构造函数和析构函数，但可以在子类中定义自己的构造函数和析构函数。
//当创建一个子类对象时，父类的构造函数会被调用，随后调用子类的构造函数；在对象销毁时，先调用子类的析构函数，然后调用父类的析构函数。