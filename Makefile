TARGET = nitcbase

ifeq ($(mode),debug)
	CFLAGS := -g
	BUILD_DIR = ./build/debug
else
	BUILD_DIR = ./build
endif

SUBDIR = Frontend_Interface Frontend Algebra Schema BlockAccess BPlusTree Cache Buffer Disk_Class

HEADERS = $(wildcard define/*.h $(foreach fd, $(SUBDIR), $(fd)/*.h))
SRCS = $(wildcard main.cpp $(foreach fd, $(SUBDIR), $(fd)/*.cpp))
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:cpp=o))

$(TARGET): $(OBJS)
	g++ $(CFLAGS) -o $@ $(OBJS) -lreadline

$(BUILD_DIR)/%.o: %.cpp $(HEADERS)
	mkdir -p $(@D)
	g++ $(CFLAGS) -o $@ -c $<

clean:
	rm -rf $(BUILD_DIR)/*