CXX = g++-15
CXXFLAGS = -std=c++23 -Iinclude -fPIC
# Iinclude - allows to get rid of relative paths in includes

LIB_NAME = libjsonparser.dylib

all: $(LIB_NAME) app

$(LIB_NAME): src/library.cpp
	$(CXX) $(CXXFLAGS) -dynamiclib -o shared/$@ $^

app: src/main.cpp $(LIB_NAME)
	mkdir -p exe
	$(CXX) $(CXXFLAGS) -Lshared -ljsonparser -Wl,-rpath,@executable_path -o exe/app.x src/main.cpp

run: all
	./app.x

clean:
	rm -f *.dylib app