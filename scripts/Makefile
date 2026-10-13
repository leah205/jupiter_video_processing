CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g -pg $(shell pkg-config --cflags opencv4) -I../wavelib/header
CXXFLAGS += -MMD -MP
-include $(wildcard *.d)
LDFLAGS = -L../wavelib/Bin
LIBS = $(shell pkg-config --libs opencv4) 
LDLIBS = -lwavelib -lwauxlib

main: main.o stack.o align.o helpers.o sharpen.o  optimisation.o videoProcessor.o test_helpers.o limit.o quality.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o main main.o stack.o quality.o align.o limit.o helpers.o sharpen.o optimisation.o  videoProcessor.o test_helpers.o $(LIBS) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# test: test.o test_helpers.o align.o test_helpers.o helpers.o
# 		$(CXX) $(CXXFLAGS) -o test test.o test_helpers.o align.o helpers.o $(LIBS)


clean:
	rm -f program *.o
