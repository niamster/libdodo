
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
			
all:
	for i in ./*; do if [ -d $$i ]; then cd $$i; make; cd ..; fi; done;

test:
	for i in ./*; do if [ -d $$i ]; then cd $$i; echo "execution of " `pwd`; ./test; cd ..; fi; done;
	
clean:
	for i in ./*; do if [ -d $$i ]; then cd $$i; make clean; cd ..; fi; done;
