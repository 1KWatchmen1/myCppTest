01two_thread.cpp和02two_thread2.cpp是两个线程交替实现打印1-100的实现。
03zhexuejia.cpp是哲学家就餐问题的实现
04base64文件夹内存在base64的encode和decode实现
06XOR运行需要openssl库(sudo apt-get install libssl-dev)：g++ cipher.cpp -o cipher -lssl -lcrypto;
1、根据密钥对明文进行异或操作，组成密文，然后对密文进行base64编码；
2、对base64编码后的密文进行解密，得到原始密文，然后在进行异或操作得到明文。
07duoxianchengExample是测试了多线程，使用了两个线程，
08duojinchengExample是三个进程，分别打印1-100；
09是多进程交替打印1-100；没实现，信号量问题