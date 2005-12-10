
#DEBUG=-g


##############################

override PREFIX:=/tmp/libdodo

##############################

MOD_MYSQL_DEF:=-DMYSQL_EXT
MOD_MYSQL_CPP:=-I/opt/mysql/include/mysql
MOD_MYSQL_LD:=-L/opt/mysql/lib/mysql -lmysqlclient

#############

MOD_PCRE_DEF:=-DPCRE_EXT
MOD_PCRE_CPP:=-I/opt/pcre/include
MOD_PCRE_LD:=-L/opt/pcre/lib -lpcre

#############

MOD_DL_DEF:=-DDL_EXT
MOD_DL_LD:=-ldl

#############

MOD_LIBXML2_DEF:=-DLIBXML2_EXT
MOD_LIBXML2_CPP:=-I/usr/include/libxml2/
MOD_LIBXML2_LD:=-lxml2

#############

MOD_CODECONV_DEF:=-DCODECONV_EXT

#############

MOD_ZLIB_DEF:=-DZLIB_EXT

#############

MOD_SQLITE_DEF:=-DSQLITE_EXT
MOD_SQLITE_CPP:=-I/opt/sqllite/include
MOD_SQLITE_LD:=-L/opt/sqllite/lib/ -lsqlite3

##############################

override DEFINES:=$(MOD_MYSQL_DEF) \
					$(MOD_PCRE_DEF) \
					$(MOD_DL_DEF) \
					$(MOD_LIBXML2_DEF) \
					$(MOD_CODECONV_DEF) \
					$(MOD_ZLIB_DEF) \
					$(MOD_SQLITE_DEF) \
					$(DEFINES)