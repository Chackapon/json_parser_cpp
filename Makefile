CXX = g++-15
CXXFLAGS = -std=c++23 -Iinclude -fPIC
# Iinclude - allows to get rid of relative paths in includes

LIB_NAME = libjsonparser.dylib

all: $(LIB_NAME) app

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

LIB_SRC = src/JSON_Node.cpp src/JSON_Parser.cpp src/JSON_Value.cpp
LIB_OBJ = $(LIB_SRC:.cpp=.o)

$(LIB_NAME): $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) -dynamiclib -o shared/$@ $^

app: src/main.cpp $(LIB_NAME)
	mkdir -p exe
	$(CXX) $(CXXFLAGS) -Lshared -ljsonparser -Wl,-rpath,@executable_path -o exe/app.x src/main.cpp

run:
	./exe/app.x

clean:
	rm -f *.dylib *.o