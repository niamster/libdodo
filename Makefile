#CC_PATH=/opt/gcc-3.4.3/bin/
#CC_PATH=/opt/gcc-4.0.1/bin/
#CC_PATH=/opt/intel/cc/9.0/bin/

CXX = $(CC_PATH)g++
#CXX = $(CC_PATH)icc

CFLAGS=-O3 -march=pentium4
OBJECTS=dbBase.o \
		tools.o \
		xexec.o \
		dbSqlBase.o \
		baseEx.o \
		dbMysql.o \
		cgiTools.o \
		regexpTools.o \
		flush.o \
		flushSocket.o \
		flushDisk.o \
		flushSTD.o \
		systemTools.o \
		timeTools.o \
		xmlTools.o \
		dbSqlite.o \
		dodoMap.o

###########################################################

include directives.mk

MODS_CPP:=$(MOD_MYSQL_CPP) \
			$(MOD_PCRE_CPP) \
			$(MOD_LIBXML2_CPP)\
			$(MOD_SQLITE_CPP)

MODS_LD:=$(MOD_MYSQL_LD) \
			$(MOD_PCRE_LD) \
			$(MOD_DL_LD) \
			$(MOD_LIBXML2_LD) \
			$(MOD_SQLITE_LD)

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
	ln -sf lib$(LIBRARY).so.$(VERSION).$(MINOR) lib$@.so
	@echo ""
	@echo ""
	@echo "Now you can run 'make install'. [PREFIX=$(PREFIX)] - change it in directives.mk if you want"
.cc.o:
	$(CXX) $(DEFINES) $(CPPFLAGS) $(CFLAGS) $(DEBUG) -Wall -fPIC -c $^
	strip -d $@

install:
	mkdir -p $(PREFIX) $(PREFIX)/lib $(PREFIX)/include/libdodo
	cp lib$(LIBRARY).so.$(VERSION).$(MINOR) $(PREFIX)/lib/
	cp -rf include/* $(PREFIX)/include/libdodo/
	ln -fs $(PREFIX)/lib/lib$(LIBRARY).so.$(VERSION).$(MINOR) $(PREFIX)/lib/lib$@.so
	@echo ""
	@echo ""
	@echo "Use libdodo with pleasure"
	
clean:
	rm -rf *.o *.so*

force : clean $(LIBRARY)
