
#DEBUG:=-g

CFLAGS:=-O3 -march=pentium4 -Wall -pipe -Wabi

##############################

override PREFIX:=/tmp/libdodo

##############################

MOD_MYSQL_CPP:=-I/opt/mysql/include/mysql
MOD_MYSQL_LD:=-L/opt/mysql/lib/mysql
MOD_MYSQL_LIB:=-lmysqlclient

#############

MOD_PCRE_CPP:=-I/opt/pcre/include
MOD_PCRE_LD:=-L/opt/pcre/lib
MOD_PCRE_LIB:=-lpcre

#############

MOD_DL_CPP:=
MOD_DL_LD:=
MOD_DL_LIB:=-ldl

#############

MOD_LIBXML2_CPP:=-I/usr/include/libxml2/
MOD_LIBXML2_LD:=
MOD_LIBXML2_LIB:=-lxml2

#############

MOD_SQLITE_CPP:=-I/opt/sqllite/include
MOD_SQLITE_LD:=-L/opt/sqllite/lib/ -lsqlite3
MOD_SQLITE_LIB:=-lsqlite3

#############

MOD_PTHREAD_CPP:=
MOD_PTHREAD_LD:=
MOD_PTHREAD_LIB:=-lpthread

#############

MOD_POSTGRESQL_CPP:=-I/opt/postgresql/include
MOD_POSTGRESQL_LD:=-L/opt/postgresql/lib/
MOD_POSTGRESQL_LIB:=-lpq

##############################
