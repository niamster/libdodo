#GCC_PATH=/opt/gcc-3.4.3/bin/

LIBTOOL = libtool
CXX = $(GCC_PATH)g++
CFLAGS=-O2 -march=pentium4

override DEFINES:=$(DEFINES)

override CPPFLAGS:=-I../../ $(CPPFLAGS)
override LDFLAGS:=-L../../ $(LDFLAGS) 

test: test.cc
	$(CXX) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) $(CFLAGS) -ldodo $^ -o $@ 
clean:
	rm -rf test .libs
