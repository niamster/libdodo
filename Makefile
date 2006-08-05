

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
		dbPostgresql.o \
		dbInterface.o \
		cgiPreprocessor.o \
		cgiFast.o \
		cgiProcessor.o

include directives.mk

###########################################################

MODS_CPP:=$(MOD_MYSQL_CPP) \
			$(MOD_PCRE_CPP) \
			$(MOD_DL_CPP) \
			$(MOD_LIBXML2_CPP)\
			$(MOD_SQLITE_CPP) \
			$(MOD_POSTGRESQL_CPP) \
			$(MOD_PTHREAD_CPP) \
			$(MOD_BZIP_CPP) \
			$(MOD_CODECONV_CPP) \
			$(MOD_FCGI_CPP) \
			$(MOD_ZLIB_CPP)

MODS_LD:=$(MOD_MYSQL_LD) \
			$(MOD_PCRE_LD) \
			$(MOD_DL_LD) \
			$(MOD_LIBXML2_LD) \
			$(MOD_SQLITE_LD) \
			$(MOD_PTHREAD_LD) \
			$(MOD_POSTGRESQL_LD) \
			$(MOD_BZIP_LD) \
			$(MOD_CODECONV_LD) \
			$(MOD_FCGI_LD) \
			$(MOD_ZLIB_LD)
			
MODS_LIB:=$(MOD_MYSQL_LIB) \
			$(MOD_PCRE_LIB) \
			$(MOD_DL_LIB) \
			$(MOD_LIBXML2_LIB) \
			$(MOD_SQLITE_LIB) \
			$(MOD_PTHREAD_LIB) \
			$(MOD_POSTGRESQL_LIB) \
			$(MOD_BZIP_LIB) \
			$(MOD_CODECONV_LIB) \
			$(MOD_FCGI_LIB) \
			$(MOD_ZLIB_LIB)
			
###########################################################

include directives.runtime.mk

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


override CPPFLAGS:=-I./include -I/usr/local/include $(MODS_CPP) $(CPPFLAGS)
override LDFLAGS:= -L./ -L/usr/local/libs $(MODS_LD) $(LDFLAGS)
override LIBS:=$(MODS_LIB)

VPATH:=src

LIBRARY:=libdodo

MAJOR:=0
MINOR:=2
RELEASE:=10

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
