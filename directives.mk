
#DEBUG=-g


##############################

override PREFIX:=/tmp/libdodo

##############################

##uncomment #define MYSQL_EXT in include/directives.h

MOD_MYSQL_CPP:=-I/opt/mysql/include/mysql
MOD_MYSQL_LD:=-L/opt/mysql/lib/mysql -lmysqlclient

#############

##uncomment #define PCRE_EXT in include/directives.h

MOD_PCRE_CPP:=-I/opt/pcre/include
MOD_PCRE_LD:=-L/opt/pcre/lib -lpcre

#############

##uncomment #define DL_EXT in include/directives.h

MOD_DL_LD:=-ldl

#############

##uncomment #define LIBXML2_EXT in include/directives.h

MOD_LIBXML2_CPP:=-I/usr/include/libxml2/
MOD_LIBXML2_LD:=-lxml2

##############################
