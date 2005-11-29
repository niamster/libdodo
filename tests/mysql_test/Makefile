#GCC_PATH=/opt/gcc-3.4.3/bin/
#GCC_PATH=/opt/gcc-4.0.1/bin/

LIBTOOL = libtool
CXX = $(GCC_PATH)g++
CFLAGS=-O2 -march=pentium4

MOD_MYSQL_CPP:=-I/opt/mysql/include/mysql

override DEFINES:=-DMYSQL_EXT $(DEFINES)

DEBUG=-g

override CPPFLAGS:=-I../../ $(CPPFLAGS)
override LDFLAGS:=-L../../ $(LDFLAGS) 

test: test.cc
	$(CXX) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) $(CFLAGS) $(DEBUG) -ldodo $^ -o $@ 

clean:
	rm -rf compare.sql .libs
