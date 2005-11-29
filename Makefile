#CC_PATH=/opt/gcc-3.4.3/bin/
#CC_PATH=/opt/gcc-4.0.1/bin/
#CC_PATH=/opt/intel/cc/9.0/bin/

CXX = $(CC_PATH)g++
#CXX = $(CC_PATH)icc

DEBUG=-g

CFLAGS=-O3 -march=pentium4
OBJECTS=dbBase.o dodoBase.o tools.o xexec.o dbSqlBase.o baseEx.o dbMysql.o cgiTools.o regexpTools.o flush.o flushSocket.o flushDisk.o flushSTD.o systemTools.o timeTools.o

override DEFINES:=$(DEFINES)

override PREFIX=/temp/libdodo

###########################################################

MOD_MYSQL_CPP:=-I/opt/mysql/include/mysql
MOD_MYSQL_LD:=-L/opt/mysql/lib/mysql -lmysqlclient

MOD_PCRE_CPP:=-I/opt/pcre/include
MOD_PCRE_LD:=-L/opt/pcre/lib -lpcre

MOD_DL_LD:=-ldl

#MOD_LIBXML2_CPP:=-I/usr/include/libxml2/
#MOD_LIBXML2_LD:=--lxml2

MODS_CPP:=$(MOD_MYSQL_CPP) $(MOD_PCRE_CPP)
MODS_LD:=$(MOD_MYSQL_LD) $(MOD_PCRE_LD) $(MOD_DL_LD)

###########################################################

override CPPFLAGS:=-I./include $(MODS_CPP) $(CPPFLAGS)
override LDFLAGS:= -L./ $(MODS_LD) $(LDFLAGS)

VPATH=src

LIBRARY=dodo
VERSION = 0.0
MINOR = 8

all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(LIBS) -shared -Wl,-soname,lib$@.so.$(VERSION).$(MINOR) -o lib$@.so.$(VERSION).$(MINOR) $^
	@echo ""
	@echo ""
	@echo "Now you can run 'make install'. [PREFIX=$(PREFIX)] - override if you want"
.cc.o:
	$(CXX) $(DEFINES) $(CPPFLAGS) $(CFLAGS) $(DEBUG) -Wall -fPIC -c $^
	#strip -d $@

install:
	mkdir -p $(PREFIX) $(PREFIX)/lib $(PREFIX)/include
	cp lib$(LIBRARY).so.$(VERSION).$(MINOR) $(PREFIX)/lib/
	cp -rf include/* $(PREFIX)/include/
	ln -fs $(PREFIX)/lib/lib$(LIBRARY).so.$(VERSION).$(MINOR) $(PREFIX)/lib/lib$@.so
	@echo ""
	@echo ""
	@echo "Use libdodo with pleasure"
	
clean:
	rm -rf *.o *.so* *.lo .libs

force : clean $(LIBRARY)
