cmake_minimum_required(VERSION 3.13)
project(aro C)

set(CMAKE_C_STANDARD 99)

add_executable(aro main.c zad2/child_manager.c zad2/mainn.c zad2/child_manager.h zad2/monitor.c zad2/monitor.h zad3/catcher.c zad3/common.cpp zad3/common.h zad3/safe_catcher.cpp zad3/safe_sender.cpp zad3/sender.cpp)