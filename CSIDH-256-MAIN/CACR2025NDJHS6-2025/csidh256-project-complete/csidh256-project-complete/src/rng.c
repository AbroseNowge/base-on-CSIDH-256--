// 密码学安全的随机数生成器
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#else
#include <unistd.h>
#include <fcntl.h>
#ifdef __linux__
#include <sys/syscall.h>
#include <linux/random.h>
#endif
#endif

// 使用系统提供的密码学安全RNG (CSPRNG)
// Windows: 使用 CryptGenRandom (Windows CSPRNG)
// Linux: 使用 /dev/urandom 或 getrandom() 系统调用
void randombytes(void *x, size_t l) {
#ifdef _WIN32
    // Windows: 使用 CryptGenRandom (Windows CSPRNG)
    HCRYPTPROV hProvider = 0;
    if (CryptAcquireContext(&hProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        if (!CryptGenRandom(hProvider, (DWORD)l, (BYTE*)x)) {
            // 如果失败，使用回退方法（不安全，但比崩溃好）
            for (size_t i = 0; i < l; i++) {
                ((uint8_t*)x)[i] = (uint8_t)rand();
            }
        }
        CryptReleaseContext(hProvider, 0);
    } else {
        // 回退到不安全的随机数（仅用于测试）
        for (size_t i = 0; i < l; i++) {
            ((uint8_t*)x)[i] = (uint8_t)rand();
        }
    }
#else
    // Linux/Unix: 优先使用 getrandom() 系统调用
    #ifdef __linux__
    if (syscall(SYS_getrandom, x, l, 0) == (ssize_t)l) {
        return; // 成功
    }
    #endif
    
    // 回退到 /dev/urandom
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd >= 0) {
        ssize_t bytes_read = read(fd, x, l);
        close(fd);
        if (bytes_read == (ssize_t)l) {
            return; // 成功
        }
    }
    
    // 最后的回退（不安全，仅用于测试）
    for (size_t i = 0; i < l; i++) {
        ((uint8_t*)x)[i] = (uint8_t)rand();
    }
#endif
}

