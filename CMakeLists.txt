set(CMAKE_C_COMPILER "gcc")

cmake_minimum_required(VERSION 3.10)
project(nitcbase)


set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} Frontend_Interface/frontend-runner.cpp
	Disk_Class/Disk.h
	Disk_Class/Disk.cpp
	Buffer/StaticBuffer.h
	Buffer/StaticBuffer.cpp
	Buffer/BlockBuffer.h
	Buffer/BlockBuffer.cpp
	Cache/RelCacheTable.h
	Cache/RelCacheTable.cpp
	Cache/AttrCacheTable.h
	Cache/AttrCacheTable.cpp
	Cache/OpenRelTable.h
	Cache/OpenRelTable.cpp
	BlockAccess/BlockAccess.h
	BlockAccess/BlockAccess.cpp
	BPlusTree/BPlusTree.h
	BPlusTree/BPlusTree.cpp
	Algebra/Algebra.h
	Algebra/Algebra.cpp
	Schema/Schema.h
	Schema/Schema.cpp
	Frontend_Interface/Frontend.cpp
	Frontend_Interface/Frontend.h
)