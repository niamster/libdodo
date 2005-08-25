LIBTOOL = libtool
CXX = g++
CFLAGS=-O3 -march=pentium4

override DEFINES:=$(DEFINES)

override CPPFLAGS:=-I/opt/mysql/include/mysql/ -I../../ $(CPPFLAGS)
override LDFLAGS:=-L../../ $(LDFLAGS) 

compare.sql: compare.sql.cc
	libtool --mode=link $(CXX) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) $(CFLAGS) -ldodo $^ -o $@ 
clean:
	rm -rf compare.sql .libs
