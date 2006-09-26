

OBJECTS:=dbBase.o \
		tools.o \
		xexec.o \
		dbSqlBase.o \
		baseEx.o \
		dbMysql.o \
		cgiTools.o \
		regexpTools.o \
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
		dbPostgresql.o \
		cgiPreprocessor.o \
		flush.o \
		dbInterface.o \
		cgiFast.o \
		cgiProcessor.o \
		flushNBA.o

###########################################################

include directives.mk

include directives.runtime.mk

include flags.mk
			
###########################################################

			
DIRECTIVES:=

ifeq ($(MOD_MYSQL),yes)
	DIRECTIVES += "\#define MYSQL_EXT\n\n"
endif

ifeq ($(MOD_PTHREAD),yes)
	DIRECTIVES += "\#define PTHREAD_EXT\n\n"
endif

ifeq ($(MOD_PCRE),yes)
	DIRECTIVES += "\#define PCRE_EXT\n\n"
endif

ifeq ($(MOD_DL),yes)
	DIRECTIVES += "\#define DL_EXT\n\n"
endif

ifeq ($(MOD_LIBXML2),yes)
	DIRECTIVES += "\#define LIBXML2_EXT\n\n"
endif

ifeq ($(MOD_SQLITE),yes)
	DIRECTIVES += "\#define SQLITE_EXT\n\n"
endif

ifeq ($(MOD_POSTGRESQL),yes)
	DIRECTIVES += "\#define POSTGRESQL_EXT\n\n"
endif

ifeq ($(MOD_BZIP),yes)
	DIRECTIVES += "\#define BZIP_EXT\n\n"
endif

ifeq ($(MOD_ZLIB),yes)
	DIRECTIVES += "\#define ZLIB_EXT\n\n"
endif

ifeq ($(MOD_CODECONV),yes)
	DIRECTIVES += "\#define CODECONV_EXT\n\n"
endif

ifeq ($(MOD_CODECONV),yes)
	DIRECTIVES += "\#define FCGI_EXT\n\n"
endif


override CPPFLAGS:=-I./include $(CPPFLAGS)
override LDFLAGS:= -L./ $(LDFLAGS)
override LIBS:=$(LIBS)

VPATH:=src

LIBRARY:=libdodo

MAJOR:=0
MINOR:=3
RELEASE:=0

all: $(LIBRARY)

$(LIBRARY): directives $(OBJECTS)
	$(CXX) $(CFLAGS) $(LDFLAGS) $(LIBS) -shared -Wl,-soname,$@.so.$(MAJOR).$(MINOR).$(RELEASE) -o $@.so.$(MAJOR).$(MINOR).$(RELEASE) $(OBJECTS)
	strip -d --strip-unneeded $(LIBRARY).so.$(MAJOR).$(MINOR).$(RELEASE)
	ln -sf $(LIBRARY).so.$(MAJOR).$(MINOR).$(RELEASE) $@.so
	
	ar rc $(LIBRARY).a.$(MAJOR).$(MINOR).$(RELEASE) $(OBJECTS)
	ln -sf $(LIBRARY).a.$(MAJOR).$(MINOR).$(RELEASE) $@.a
	
	@echo ""
	@echo ""
	@echo "Now you can run 'gmake install'. [PREFIX=$(PREFIX)] - change it in directives.mk if you want"

directives:
	echo -e $(DIRECTIVES) > include/directives.runtime.h
	
.cc.o:
	$(CXX) $(DEFINES) $(CPPFLAGS) $(CFLAGS) $(DEBUG) -fPIC -c $^
	strip -d --strip-unneeded $@

install: all
	mkdir -p $(PREFIX) $(PREFIX)/lib $(PREFIX)/include/$(LIBRARY)
	cp $(LIBRARY).so.$(MAJOR).$(MINOR).$(RELEASE) $(PREFIX)/lib/
	cp -rf include/* $(PREFIX)/include/$(LIBRARY)/
	
	ln -fs $(PREFIX)/lib/$(LIBRARY).so.$(MAJOR).$(MINOR).$(RELEASE) $(PREFIX)/lib/$(LIBRARY).so
	ldconfig -n $(PREFIX)/lib/
	
	@echo ""
	@echo ""
	@echo "Use libdodo with pleasure"
	
clean:
	rm -rf *.o *.so* *.a*

force : clean $(LIBRARY)
