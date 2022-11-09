default: nitcbase

nitcbase: Algebra/* BlockAccess/* BPlusTree/* Buffer/* Cache/* define/* Disk_Class/* Frontend/* Frontend_Interface/* Schema/* main.cpp
	g++ Algebra/*.cpp BlockAccess/*.cpp BPlusTree/*.cpp Buffer/*.cpp Cache/*.cpp Disk_Class/*.cpp Frontend/*.cpp Frontend_Interface/*.cpp Schema/*.cpp main.cpp -o nitcbase

clean:
	$(RM) nitcbase *.o