/*
 * @FileName: 
 * @Description: 
 * @Autor: Liujunjie/Aries-441
 * @StudentNumber: 521021911059
 * @Date: 2023-12-03 23:49:46
 * @E-mail: sjtu.liu.jj@gmail.com/sjtu.1518228705@sjtu.edu.cn
 * @LastEditTime: 2023-12-05 16:13:21
 */
#include <stdio.h>
#include <openssl/sha.h>

int main() {
    unsigned char input[] = "Hello, World!";   // 要计算 SHA256 哈希值的消息
    unsigned char output[SHA256_DIGEST_LENGTH]; // 存储计算结果的哈希值

    // 使用 SHA256 函数计算哈希值
    SHA256(input, sizeof(input) - 1, output);

    // 打印哈希值
    printf("SHA256 Hash: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", output[i]);
    }
    printf("\n");

    return 0;
}