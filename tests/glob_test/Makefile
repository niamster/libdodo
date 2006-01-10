#GCC_PATH:=/opt/gcc-3.4.3/bin/
#GCC_PATH:=/opt/gcc-4.0.1/bin/

CXX:=$(GCC_PATH)g++
CFLAGS=-O2 -march=pentium4

override DEFINES:=$(DEFINES)

#DEBUG:=-g

override CPPFLAGS:=-I../../include $(CPPFLAGS)
override LDFLAGS:=-L../../ $(LDFLAGS) 

all: test module

test: test.cc
	$(CXX) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) $(CFLAGS) $(DEBUG) -ldodo $^ -o $@ 

module: module.cc
	$(CXX) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) $(CFLAGS) $(DEBUG) -Wall -shared -ldodo $^ -o $@ 

clean:
	rm -fr test module
