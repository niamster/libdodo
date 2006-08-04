#GCC_PATH:=/opt/gcc-3.4.3/bin/
#GCC_PATH:=/opt/gcc-4.0.1/bin/
CXX:=$(GCC_PATH)g++

#DEBUG:=-g

CFLAGS:=-O3 -march=pentium4 -Wall -pipe

DEFINES:=-D_REENTRANT -D_GNU_SOURCE -D__USE_UNIX98 -D_GNU_SOURCE

##############################

override PREFIX:=/opt/libdodo

##############################

MOD_MYSQL:=yes
MOD_MYSQL_CPP:=-I/opt/mysql/include/mysql
MOD_MYSQL_LD:=-L/opt/mysql/lib/mysql
MOD_MYSQL_LIB:=-lmysqlclient

#############

MOD_PCRE:=yes
MOD_PCRE_CPP:=-I/opt/pcre/include
MOD_PCRE_LD:=-L/opt/pcre/lib
MOD_PCRE_LIB:=-lpcre

#############

MOD_DL:=yes
MOD_DL_CPP:=
MOD_DL_LD:=
MOD_DL_LIB:=-ldl

#############

MOD_LIBXML2:=yes
MOD_LIBXML2_CPP:=-I/usr/include/libxml2/
MOD_LIBXML2_LD:=
MOD_LIBXML2_LIB:=-lxml2

#############

MOD_SQLITE:=yes
MOD_SQLITE_CPP:=-I/opt/sqllite/include
MOD_SQLITE_LD:=-L/opt/sqllite/lib/ -lsqlite3
MOD_SQLITE_LIB:=-lsqlite3

#############

MOD_PTHREAD:=yes
MOD_PTHREAD_LD:=
MOD_PTHREAD_CPP:=
MOD_PTHREAD_LIB:=-lpthread

#############

MOD_POSTGRESQL:=yes
MOD_POSTGRESQL_CPP:=-I/opt/postgresql/include
MOD_POSTGRESQL_LD:=-L/opt/postgresql/lib/
MOD_POSTGRESQL_LIB:=-lpq

#############

MOD_BZIP:=yes
MOD_BZIP_CPP:=
MOD_BZIP_LD:=
MOD_BZIP_LIB:=-lbz2

#############

MOD_ZLIB:=yes
MOD_ZLIB_CPP:=
MOD_ZLIB_LD:=
MOD_ZLIB_LIB:=

#############

MOD_CODECONV:=yes
MOD_CODECONV_CPP:=
MOD_CODECONV_LD:=
MOD_CODECONV_LIB:=

##############################
