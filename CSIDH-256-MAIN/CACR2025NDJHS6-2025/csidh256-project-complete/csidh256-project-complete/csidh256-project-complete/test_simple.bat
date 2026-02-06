@echo off
echo 测试MSYS2环境...
F:\msys64\mingw64.exe -c "echo 'MSYS2环境正常' && pwd"

echo.
echo 测试编译...
F:\msys64\mingw64.exe -c "cd '/c/Users/20513/Desktop/csidh256-project-complete/csidh256-project-complete/csidh256-project-complete' && gcc -O3 -std=c99 -I../../src crypto_test_vectors_generator.c ../../src/mont_field.c ../../src/mont_field_optimized.c ../../src/traditional_mul.c -o test_simple.exe"

echo.
echo 测试运行...
F:\msys64\mingw64.exe -c "cd '/c/Users/20513/Desktop/csidh256-project-complete/csidh256-project-complete/csidh256-project-complete' && echo '1' | ./test_simple.exe"

echo.
echo 测试完成！
pause


