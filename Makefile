
CXXFLAGS = -std=c++11 -Iinclude -fPIC

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	CXX = g++-15
    LIB_NAME = libjsonparser.dylib
    SHARED_FLAG = -dynamiclib
    RPATH = -Wl,-rpath,@executable_path/../shared
else ifeq ($(UNAME_S),Linux)
	CXX = g++
    LIB_NAME = libjsonparser.so
    SHARED_FLAG = -shared
    RPATH = -Wl,-rpath,'$$ORIGIN/../shared'
else
    $(error Unsupported OS: $(UNAME_S))
endif

LIB_PATH = shared/$(LIB_NAME)
APP_PATH = exe/app.x

.PHONY: all run clean

all: $(LIB_PATH) $(APP_PATH)

$(LIB_PATH): src/JSON_Parser.cpp src/JSON_Node.cpp src/JSON_Value.cpp
	mkdir -p shared
	$(CXX) $(CXXFLAGS) $(SHARED_FLAG) -o $@ $^

$(APP_PATH): src/main.cpp $(LIB_PATH)
	mkdir -p exe
	$(CXX) $(CXXFLAGS) src/main.cpp $(LIB_PATH) $(RPATH) -o $@

run: all
	./$(APP_PATH)

clean:
	rm -rf shared exe