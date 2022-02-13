cmake_minimum_required(VERSION 3.10)
project(Frontend-Interface)

set(CMAKE_CXX_STANDARD 17)

add_executable(Frontend-Interface frontend-runner.cpp ../Disk_Class/Disk.cpp Frontend.cpp Frontend.h ../Buffer/StaticBuffer.cpp ../Buffer/BlockBuffer.cpp )