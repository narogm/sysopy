cmake_minimum_required(VERSION 3.13)
project(zad2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(zad2
        files_list
        main.c
        Makefile
        monitor.c
        monitor.h
        tester.c
        testfile
        testfile2)
