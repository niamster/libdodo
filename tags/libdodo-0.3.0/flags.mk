
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
			
override CPPFLAGS:=-I/usr/local/include $(MODS_CPP) $(CPPFLAGS)
override LDFLAGS:=-L/usr/local/libs $(MODS_LD) $(LDFLAGS)
override LIBS:=$(MODS_LIB)			
