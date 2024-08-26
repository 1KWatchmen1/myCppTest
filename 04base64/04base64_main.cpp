#include <iostream>
#include <string>
#include "04base64.h"

void test_base64_encode_decode() {
    std::string test_str = "Hello, World!";
    std::string encoded_str = base64_encode(test_str);
    std::string decoded_str = base64_decode(encoded_str);
    
    std::cout << "Original string: " << test_str << std::endl;
    std::cout << "Base64 encoded: " << encoded_str << std::endl;
    std::cout << "Base64 decoded: " << decoded_str << std::endl;

    std::string another_test_str = "C++ Base64 Encoding";
    std::string another_encoded_str = base64_encode(another_test_str);
    std::string another_decoded_str = base64_decode(another_encoded_str);
    std::cout << "Original string: " << another_test_str << std::endl;
    std::cout << "Base64 encoded: " << another_encoded_str << std::endl;
    std::cout << "Base64 decoded: " << another_decoded_str << std::endl;
}

int main() {
    test_base64_encode_decode();
    return 0;
}
