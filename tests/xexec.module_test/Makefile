
include ../../directives.mk

###########################################################

MODS_CPP:=$(MOD_MYSQL_CPP) \
			$(MOD_PCRE_CPP) \
			$(MOD_DL_CPP) \
			$(MOD_LIBXML2_CPP)\
			$(MOD_SQLITE_CPP) \
			$(MOD_POSTGRESQL_CPP) \
			$(MOD_PTHREAD_CPP) \
			$(MOD_BZIP_CPP)
			
###########################################################

override CPPFLAGS:=-I../../include -I/usr/local/include $(MODS_CPP) $(CPPFLAGS)
override LDFLAGS:=-L../../ $(LDFLAGS) 

all: test module

test: test.cc
	$(CXX) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) $(CFLAGS) $(DEBUG) -ldodo $^ -o $@ 

module: module.cc
	$(CXX) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) $(CFLAGS) $(DEBUG) -Wall -shared -ldodo -fPIC $^ -o $@ 
	
clean:
	rm -rf test module
