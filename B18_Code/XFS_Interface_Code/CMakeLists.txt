cmake_minimum_required(VERSION 3.10)
project(XFS-Interface)

set(CMAKE_CXX_STANDARD 17)

add_executable(XFS-Interface interface.cpp Disk.cpp schema.cpp block_access.cpp external_fs_commands.cpp OpenRelTable.cpp algebra.cpp)