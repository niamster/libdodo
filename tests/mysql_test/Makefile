GCC_PATH=/opt/gcc-3.4.3/

LIBTOOL = libtool
CXX = $(GCC_PATH)/bin/g++
CFLAGS=-O2 -march=pentium4

override DEFINES:=$(DEFINES)

override CPPFLAGS:=-I/opt/mysql/include/mysql/ -I../../ $(CPPFLAGS)
override LDFLAGS:=-L../../ $(LDFLAGS) 

compare.sql: compare.sql.cc
	libtool --mode=link $(CXX) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) $(CFLAGS) -ldodo $^ -o $@ 
clean:
	rm -rf compare.sql .libs
