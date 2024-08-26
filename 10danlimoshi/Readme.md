01-v:
_static Singleton1* _instance;:存储在全局未初始化区，单这是个指针，指向的资源是在堆上；由于全局静态区的空间自动释放，但是堆上的资源需要手动释放；所以这个版本的对象不能够被释放；
02_v:
在01-v的基础上释放了堆上的资源，这个版本是单线程下的完整版本；
03_V:多线程下线程安全的（线程安全指的是对象的创建或者说获取是线程安全的，并非是对象中的操作时线程安全的）
所以只需要在GetInstance中加上锁即可；
这里的锁用了RAII的思想，锁在离开他的作用空间就被自动释放了（对象也被销毁了）
04_v:双检测
在03_v中由于_instance = new Singleton3();只执行一次，但是每次访问得到实例却都要加锁，浪费；所以加一个双检测(样可以减少不必要的锁操作。)；如果没有双检测下面的那个检测，可能在刚开始创建的时候并发线程进来创建多个实例；
但是依旧在多处理器多核心的时候时候问题的；因为在_instance = new Singleton3();创建时，有三步：1、分配内存、2、调用构造函数3、返回对象指针；在多核心的时候可能会出现运行时CPU指令重排；
有可能上面3步骤执行顺序打乱，还没有执行构造函数就已经返回了指针，此时_instance并没有进行创建；
在多核心duo处理器下就要用原子操作和内存屏障来解决；
05_v:使用原子变量：解决v4在多核心下，_instance没有进行创建就已经使用了地问题
内存屏障解决两个问题：规定内存序（）；可见性问题（所核）
两个屏障解决tmp = new Singleton2;的优化不会外溢，但还是有指令重排现象；但是不会外溢到屏障外；
Singleton2* tmp = _instance.load(std::memory_order_relaxed);///拿到最新数据
_instance.store(tmp,std::memory_order_relaxed);//写入数据

06:C++11环境下才可以，
C++11下静态局部变量有一个线程安全的特性；
静态变量也是懒汉模式，也不需要考虑CPU指令重拍的问题；
存储在全局静态区，内存自动释放，当程序退出的时候，调用析构函数；

07是封装，只要接口就能用；
使用模版类进行封装，对于使用者而言，只需要写一个子类，继承这个模版类即可；
07中DesignPattern* s1 = DesignPattern::GetInstance();这一句的执行流程：
第一次调用：
1、DesignPattern 作为模板参数被传递给 Singleton 模板类。
2、静态变量 instance：在 GetInstance 中，static T instance; 被执行，这会创建 DesignPattern 类型的 instance。
3、构造函数：DesignPattern 的构造函数被调用。
4、返回实例地址：GetInstance 返回 &instance，即 DesignPattern 类型的对象地址。
后续调用：
1、静态变量已存在：后续调用 GetInstance() 时，static T instance; 不会再执行，因为 instance 已经被创建。
2、直接返回地址：GetInstance 直接返回已经创建的 instance 的地址。
