#GCC_PATH:=/opt/gcc-3.4.3/bin/
#GCC_PATH:=/opt/gcc-4.0.1/bin/

CXX:=$(GCC_PATH)g++
CFLAGS:=-O2 -march=pentium4

MOD_SQLITE_CPP:=-I/opt/sqllite/include/

override DEFINES:=$(DEFINES)

#DEBUG:=-g

override CPPFLAGS:=-I../../include $(MOD_SQLITE_CPP) $(CPPFLAGS)
override LDFLAGS:=-L../../ $(LDFLAGS) 

test: test.cc
	$(CXX) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) $(CFLAGS) $(DEBUG) -ldodo $^ -o $@ 

clean:
	rm -rf test test.lite
