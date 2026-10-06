CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g -pg $(shell pkg-config --cflags opencv4)
LIBS = $(shell pkg-config --libs opencv4)

main: main.o stack.o align.o helpers.o sharpen.o  optimisation.o videoProcessor.o test_helpers.o limit.o
	$(CXX) $(CXXFLAGS) -o main main.o stack.o  align.o limit.o helpers.o sharpen.o optimisation.o  videoProcessor.o test_helpers.o $(LIBS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

stack.o: stack.cpp stack.h frameInfo.h
	$(CXX) $(CXXFLAGS) -c stack.cpp

align.o: align.cpp align.h frameInfo.h
	$(CXX) $(CXXFLAGS) -c align.cpp

helpers.o: helpers.cpp helpers.h frameInfo.h
	$(CXX) $(CXXFLAGS) -c helpers.cpp

sharpen.o: sharpen.cpp sharpen.h frameInfo.h
	$(CXX) $(CXXFLAGS) -c sharpen.cpp

optimisation.o: optimisation.cpp optimisation.h frameInfo.h
		$(CXX) $(CXXFLAGS) -c optimisation.cpp

videoProcessor.o: videoProcessor.cpp videoProcessor.h frameInfo.h
			$(CXX) $(CXXFLAGS) -c videoProcessor.cpp

test: test.o test_helpers.o align.o test_helpers.o helpers.o
		$(CXX) $(CXXFLAGS) -o test test.o test_helpers.o align.o helpers.o $(LIBS)

test.o: test.cpp 
		$(CXX) $(CXXFLAGS) -c test.cpp

test_helpers.o: test_helpers.cpp test_helpers.h 
	$(CXX) $(CXXFLAGS) -c test_helpers.cpp

limit.o: limit.cpp frameInfo.h
		$(CXX) $(CXXFLAGS) -c limit.cpp



clean:
	rm -f program *.o
