cmake_minimum_required(VERSION 3.19)
project(B18_Code)

set(CMAKE_CXX_STANDARD 17)

add_executable(B18_Code interface_new.cpp Disk.cpp schema.cpp block_access.cpp external_fs_commands.cpp)