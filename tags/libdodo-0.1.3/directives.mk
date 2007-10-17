
#DEBUG=-g

##############################

override PREFIX:=/tmp/libdodo

##############################

MOD_MYSQL_CPP:=-I/opt/mysql/include/mysql
MOD_MYSQL_LD:=-L/opt/mysql/lib/mysql -lmysqlclient

#############

MOD_PCRE_CPP:=-I/opt/pcre/include
MOD_PCRE_LD:=-L/opt/pcre/lib -lpcre

#############

MOD_DL_LD:=-ldl

#############

MOD_LIBXML2_CPP:=-I/usr/include/libxml2/
MOD_LIBXML2_LD:=-lxml2

#############

MOD_SQLITE_CPP:=-I/opt/sqllite/include
MOD_SQLITE_LD:=-L/opt/sqllite/lib/ -lsqlite3

#############

MOD_PTHREAD_LD:=-lpthread -pthread

##############################