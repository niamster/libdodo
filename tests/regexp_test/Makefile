#GCC_PATH=/opt/gcc-3.4.3/bin/
GCC_PATH=/opt/gcc-4.0.1/bin/

LIBTOOL = libtool
CXX = $(GCC_PATH)g++
CFLAGS=-O2 -march=pentium4

#DEBUG=-g

override DEFINES:=$(DEFINES)

override CPPFLAGS:=-I../../include -I/opt/pcre/include $(CPPFLAGS)
override LDFLAGS:=-L../../ $(LDFLAGS) 

test: test.cc
	$(CXX) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) $(CFLAGS) $(DEBUG) -ldodo $^ -o $@ 

clean:
	rm -rf test .libs
