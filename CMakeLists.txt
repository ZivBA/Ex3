cmake_minimum_required(VERSION 3.2)
project(Ex3)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wvla -std=c99")

set(SOURCE_FILES  MyString.h mystring_demo.c mystring_demo.h )
add_executable(Ex3 ${SOURCE_FILES}  MyString.c MyStringMain.c)