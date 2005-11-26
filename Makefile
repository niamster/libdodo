#CC_PATH=/opt/gcc-3.4.3/bin/
#CC_PATH=/opt/gcc-4.0.1/bin/
#CC_PATH=/opt/intel/cc/9.0/bin/

CXX = $(CC_PATH)g++
#CXX = $(CC_PATH)icc

CFLAGS=-O2 -march=pentium4
OBJECTS=dbBase.o dodoBase.o tools.o xexec.o sqlBase.o baseEx.o mysqlpp.o cgipp.o regexp.o flush.o flushSocket.o flushDisk.o flushSTD.o systemTools.o

override DEFINES:=-DLIB_COMPILE -DMYSQL_EXT -DPCRE_EXT $(DEFINES)

MOD_MYSQL_CPP:=-I/opt/mysql/include/mysql
MOD_MYSQL_LD:=-L/opt/mysql/lib/mysql -lmysqlclient

MOD_PCRE_CPP:=-I/opt/pcre/include
MOD_PCRE_LD:=-L/opt/pcre/lib -lpcre

MOD_DL_LD:=-ldl

override CPPFLAGS:=-I./ $(MOD_MYSQL_CPP) $(MOD_PCRE_CPP) $(CPPFLAGS)
override LDFLAGS:= -L./ $(MOD_MYSQL_LD) $(MOD_PCRE_LD) $(MOD_DL_LD) $(LDFLAGS)

LIBRARY=dodo

VERSION = 0.0

MINOR = 8

all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(LIBS) -shared -Wl,-soname,lib$@.so.$(VERSION).$(MINOR) -o lib$@.so.$(VERSION).$(MINOR) $^
	ln -fs lib$@.so.$(VERSION).$(MINOR) lib$@.so
	ldconfig -n ./
.cc.o:
	$(CXX) $(DEFINES) $(CPPFLAGS) $(CFLAGS) -Wall -fPIC -c $^
	strip -d $@
	
clean:
	rm -rf *.o *.so* *.lo .libs

force : clean $(LIBRARY)
