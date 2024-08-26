#include <iostream>

class Singleton6
{
public:
    static Singleton6* GetInstance()
    {
        static Singleton6 instance;
        return &instance;
    }
private:
    Singleton6(){}
    ~Singleton6(){
        std::cout << "~Singleton6()\n";
    }
    Singleton6(const Singleton6 &) = delete;//拷贝构造函数
    Singleton6& operator=(const Singleton6&) = delete;//拷贝赋值构造函数
    Singleton6(Singleton6 &&) = delete;//移动拷贝构造函数
    Singleton6& operator=(Singleton6 &&) = delete;//移动拷贝赋值构造函数

};

int main()
{
    Singleton6 *s1 = Singleton6::GetInstance();
    return 0;
}