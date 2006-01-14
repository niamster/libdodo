#CC_PATH:=/opt/gcc-3.4.3/bin/
#CC_PATH:=/opt/gcc-4.0.1/bin/
#CC_PATH:=/opt/intel/cc/9.0/bin/

CXX:=$(CC_PATH)g++
#CXX:=$(CC_PATH)icpc

CFLAGS:=-O3 -march=pentium4 -Wall

OBJECTS:=dbBase.o \
		tools.o \
		xexec.o \
		dbSqlBase.o \
		baseEx.o \
		dbMysql.o \
		cgiTools.o \
		regexpTools.o \
		flush.o \
		flushSocketOptions.o \
		flushSocketExchange.o \
		flushSocketTools.o \
		flushSocket.o \
		flushDiskTools.o \
		flushDisk.o \
		flushSTD.o \
		systemTools.o \
		timeTools.o \
		xmlTools.o \
		dbSqlite.o \
		systemThreads.o \
		systemThreadShares.o \
		dbPostgresql.o

###########################################################

include directives.mk

MODS_CPP:=$(MOD_MYSQL_CPP) \
			$(MOD_PCRE_CPP) \
			$(MOD_LIBXML2_CPP)\
			$(MOD_SQLITE_CPP) \
			$(MOD_POSTGRESQL_CPP)

MODS_LD:=$(MOD_MYSQL_LD) \
			$(MOD_PCRE_LD) \
			$(MOD_DL_LD) \
			$(MOD_LIBXML2_LD) \
			$(MOD_SQLITE_LD) \
			$(MOD_PTHREAD_LD) \
			$(MOD_POSTGRESQL_LD)

###########################################################

override CPPFLAGS:=-I./include $(MODS_CPP) $(CPPFLAGS)
override LDFLAGS:= -L./ $(MODS_LD) $(LDFLAGS)

VPATH:=src

LIBRARY:="libdodo"

MAJOR:=0
MINOR:=1
RELEASE:=7

all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(LIBS) $(CFLAGS) -shared -Wl,-soname,$@.so.$(MAJOR).$(MINOR).$(RELEASE) -o $@.so.$(MAJOR).$(MINOR).$(RELEASE) $^
	strip -d --strip-unneeded $(LIBRARY).so.$(MAJOR).$(MINOR).$(RELEASE)
	ln -sf $(LIBRARY).so.$(MAJOR).$(MINOR).$(RELEASE) $@.so
	@echo ""
	@echo ""
	@echo "Now you can run 'gmake install'. [PREFIX=$(PREFIX)] - change it in directives.mk if you want"
.cc.o:
	$(CXX) $(DEFINES) $(CPPFLAGS) $(CFLAGS) $(DEBUG) -fPIC -c $^
	strip -d --strip-unneeded $@

install:
	mkdir -p $(PREFIX) $(PREFIX)/lib $(PREFIX)/include/$(LIBRARY)
	cp $(LIBRARY).so.$(MAJOR).$(MINOR).$(RELEASE) $(PREFIX)/lib/
	cp -rf include/* $(PREFIX)/include/$(LIBRARY)/
	ln -fs $(PREFIX)/lib/$(LIBRARY).so.$(MAJOR).$(MINOR).$(RELEASE) $(PREFIX)/lib/$(LIBRARY).so
	@echo ""
	@echo ""
	@echo "Use libdodo with pleasure"
	
clean:
	rm -rf *.o *.so*

force : clean $(LIBRARY)
