CXX      = clang++
CXXFLAGS = -std=c++17

all: tests dsataserver

tests: tests.c
	$(CXX) $(CXXFLAGS) -o tests tests.c

dsataserver: dsataserver_main.cpp
	$(CXX) $(CXXFLAGS) -o dsataserver dsataserver_main.cpp

clean:
	rm -f tests dsataserver
