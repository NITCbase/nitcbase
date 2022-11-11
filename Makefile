TARGET = nitcbase

SUBDIR = Frontend_Interface Frontend Algebra Schema BlockAccess BPlusTree Cache Buffer Disk_Class
BUILD_DIR = ./build

HEADERS = $(wildcard define/*.h $(foreach fd, $(SUBDIR), $(fd)/*.h))
SRCS = $(wildcard main.cpp $(foreach fd, $(SUBDIR), $(fd)/*.cpp))
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:cpp=o))

$(TARGET): $(OBJS)
	g++ -o $@ $(OBJS)

$(BUILD_DIR)/%.o: %.cpp $(HEADERS)
	mkdir -p $(@D)
	g++ -o $@ -c $<

clean:
	rm -rf $(BUILD_DIR)/*