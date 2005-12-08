#GCC_PATH=/opt/gcc-3.4.3/bin/
GCC_PATH=/opt/gcc-4.0.1/bin/

LIBTOOL = libtool
CXX = $(GCC_PATH)g++
CFLAGS=-O2 -march=pentium4

MOD_MYSQL_CPP:=-I/opt/mysql/include/mysql

override DEFINES:=$(DEFINES)

#DEBUG=-g

override CPPFLAGS:=-I../../include $(MOD_MYSQL_CPP) $(CPPFLAGS)
override LDFLAGS:=-L../../ $(LDFLAGS) 

test: test.cc
	$(CXX) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) $(CFLAGS) $(DEBUG) -ldodo $^ -o $@ 

compare: compare.sql.cc
	$(CXX) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) $(CFLAGS) $(DEBUG) -ldodo $^ -o $@ 
	
clean:
	rm -rf test compare .libs
