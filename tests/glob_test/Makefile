LIBTOOL = libtool
CXX = g++
CFLAGS=-O3 -march=pentium4

override DEFINES:=$(DEFINES)

override CPPFLAGS:=-I/opt/mysql/include/mysql/ -I../../ $(CPPFLAGS)
override LDFLAGS:=-L../../ $(LDFLAGS) 

test: test.cc
	libtool --mode=link $(CXX) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) $(CFLAGS) -ldodo $^ -o $@ 
clean:
	rm -fr test .libs core*
