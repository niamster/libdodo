#CC_PATH=/opt/gcc-3.4.3/bin/
#CC_PATH=/opt/gcc-4.0.1/bin/
#CC_PATH=/opt/intel/cc/9.0/bin/

CXX = $(CC_PATH)g++
#CXX = $(CC_PATH)icc

CFLAGS=-O2 -march=pentium4
OBJECTS=dbBase.o dodoBase.o tools.o xexec.o dbSqlBase.o baseEx.o dbMysql.o cgiTools.o regexpTools.o flush.o flushSocket.o flushDisk.o flushSTD.o systemTools.o timeTools.o

override DEFINES:=-DMYSQL_EXT -DPCRE_EXT $(DEFINES)

MOD_MYSQL_CPP:=-I/opt/mysql/include/mysql
MOD_MYSQL_LD:=-L/opt/mysql/lib/mysql -lmysqlclient

MOD_PCRE_CPP:=-I/opt/pcre/include
MOD_PCRE_LD:=-L/opt/pcre/lib -lpcre

MOD_DL_LD:=-ldl

#MOD_LIBXML2_CPP:=-I/usr/include/libxml2/
#MOD_LIBXML2_LD:=--lxml2

MODS_CPP:=$(MOD_MYSQL_CPP) $(MOD_PCRE_CPP)
MODS_LD:=$(MOD_MYSQL_LD) $(MOD_PCRE_LD) $(MOD_DL_LD)

override CPPFLAGS:=-I./ $(MODS_CPP) $(CPPFLAGS)
override LDFLAGS:= -L./ $(MODS_LD) $(LDFLAGS)

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
