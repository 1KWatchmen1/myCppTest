#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <bitset>
#include <cstring>
#include <openssl/bio.h>//使用 OpenSSL 库中的 BIO 函数来处理 Base64 编码和解码。你需要在编译时链接 OpenSSL 库（-lssl -lcrypto）。
#include <openssl/evp.h>
#include <openssl/buffer.h>//BUF_MEM 结构体是一个不完整类型（即前向声明），在使用之前需要包含相应的头文件。

// Base64 编码和解码的函数
std::string Base64Encode(const std::string& input) {
    BIO *bio, *b64;
    BUF_MEM *bptr;
    std::string output;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_write(bio, input.data(), input.size());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bptr);

    output.assign(bptr->data, bptr->length - 1);
    BIO_free_all(bio);
    return output;
}



//BIO_read 函数在读取解码后的数据时可能没有正确处理输入的长度，或者输出的字符串长度没有正确计算。
std::string Base64Decode(const std::string &input) {
    BIO *bio, *b64;
    char *buffer = (char*)malloc(input.length());
    memset(buffer, 0, input.length());

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(input.c_str(), input.length());
    BIO_push(b64, bio);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    int decoded_size = BIO_read(b64, buffer, input.length());
    BIO_free_all(b64);

    std::string output(buffer, decoded_size);
    free(buffer);
    return output;
}


// XOR 加密和解密函数:XORCipher 函数执行 XOR 操作来加密或解密数据。
std::string XORCipher(const std::string& text, const std::string& key) {
    std::string result(text.size(), '\0');
    size_t keyLength = key.size();
    for (size_t i = 0; i < text.size(); ++i) {
        result[i] = text[i] ^ key[i % keyLength];
    }
    return result;
}
//Encode 函数先进行 XOR 加密，然后进行 Base64 编码。
std::string Encode(const std::string& plainText, const std::string& key) {
    std::string encryptedText = XORCipher(plainText, key);
    return Base64Encode(encryptedText);
}
//Decode 函数先进行 Base64 解码，然后进行 XOR 解密。
std::string Decode(const std::string& cipherText, const std::string& key) {
    std::string decodedBase64 = Base64Decode(cipherText);
    return XORCipher(decodedBase64, key);
}

int main() {
    std::string key = "3567d8cndkei%*x9(-32[]KDF(32222";
    std::string plainText = "Hello World!";

    std::string encodedText = Encode(plainText, key);
    std::cout << "Encoded: " << encodedText << std::endl;

    std::string decodedText = Decode(encodedText, key);
    std::cout << "Decoded: " << decodedText << std::endl;

    return 0;
}
