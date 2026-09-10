CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g -pg $(shell pkg-config --cflags opencv4)
LIBS = $(shell pkg-config --libs opencv4)

main: main.o stack.o align.o helpers.o sharpen.o normalize.o
	$(CXX) $(CXXFLAGS) -o main main.o stack.o  align.o helpers.o sharpen.o $(LIBS)

main.o: main.cpp stack.h
	$(CXX) $(CXXFLAGS) -c main.cpp

stack.o: stack.cpp stack.h
	$(CXX) $(CXXFLAGS) -c stack.cpp

align.o: align.cpp align.h
	$(CXX) $(CXXFLAGS) -c align.cpp

helpers.o: helpers.cpp helpers.h
	$(CXX) $(CXXFLAGS) -c helpers.cpp

sharpen.o: sharpen.cpp sharpen.h
	$(CXX) $(CXXFLAGS) -c sharpen.cpp

normalize.o: normalize.cpp normalize.h
	$(CXX) $(CXXFLAGS) -c normalize.cpp

clean:
	rm -f program *.o
