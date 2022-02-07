cmake_minimum_required(VERSION 3.19)
project(AES C)

set(CMAKE_C_STANDARD 11)

add_executable(AES
               src/include/SBox.h
               src/include/ShiftRows.h src/ShiftRows.c
               src/include/MixColumns.h src/MixColumns.c
               src/include/AddRoundKey.h src/AddRoundKey.c
               src/include/AES.h src/AES.c
               src/include/SubBytes.h src/SubBytes.c
               src/main.c)
