CMAKE_MINIMUM_REQUIRED(VERSION 2.4.6)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
CMAKE_POLICY(SET CMP0005 NEW)

##
# project identity
##

PROJECT(libdodo)
SET(PROJECT_VERSION 0.11.0)
SET(PACKAGE_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")

ADD_DEFINITIONS("-DPACKAGE_NAME=\"${PROJECT_NAME}\"")
ADD_DEFINITIONS("-DPACKAGE_VERSION=\"${PROJECT_VERSION}\"")
ADD_DEFINITIONS("-DPACKAGE_STRING=\"${PACKAGE_STRING}\"")

##
# default options
##

SET(LIBDODO_INSTALL_PREFIX "/usr" CACHE PATH "Prefix prepended to install directories")
SET(CMAKE_INSTALL_PREFIX "${LIBDODO_INSTALL_PREFIX}" CACHE PATH "Prefix prepended to install directories" FORCE)

##
# build options help
##
OPTION(HELP "Enable profiling(ON/OFF)" OFF)
IF(HELP)
	SET(HELP OFF CACHE BOOL "display build options" FORCE)
	MESSAGE(STATUS "
	CMake configures ${PACKAGE_STRING} to adapt to many kinds of systems.

	Usage: cmake [OPTION]... [VAR=VALUE]...
	Configuration:
		-DHELP=ON				display this help and exit
	
	Installation directories:
		-DLIBDODO_INSTALL_PREFIX=PREFIX		install architecture-independent 
							files in PREFIX[${LIBDODO_INSTALL_PREFIX}]
	
	By default, `make install' will install all the files in
	`${LIBDODO_INSTALL_PREFIX}/bin', `${LIBDODO_INSTALL_PREFIX}/lib' etc.
	You can specify	an installation prefix other than `${LIBDODO_INSTALL_PREFIX}' 
	using `-DLIBDODO_INSTALL_PREFIX', for instance `-DLIBDODO_INSTALL_PREFIX:PATH=$HOME'.

	Optional Features:
		-DFEATURE=OFF				do not include FEATURE
		-DFEATURE=ON				include FEATURE

		-DENABLE_DEBUG=[ON|OFF]			enable debug info, OFF by default
		-DENABLE_PROFILING=[ON|OFF]		enable profiling, OFF by default
		-DENABLE_LIBWRAP=[ON|OFF]		enable libwrap, ON by default
		-DENABLE_DEQUE=[ON|OFF]			use std::deque instead of std::vector, ON by default
		-DENABLE_IO_XEXEC=[ON|OFF]		compile io:: with xexec support, ON by default
		-DENABLE_DB_XEXEC=[ON|OFF]		compile db:: with xexec support, ON by default
		-DENABLE_GRAPHICS_XEXEC=[ON|OFF]		compile graphics:: with xexec support, ON by default
		-DENABLE_DL_LIB=[ON|OFF]		enable dl as lib, ON by default
		-DENABLE_FAST_DYNAMIC_LINKING=[ON|OFF] 	do not unload library to produce better performance, ON by default
		-DENABLE_ZLIB_LIB=[ON|OFF]		use zlib as lib, OFF by default
		-DENABLE_ICONV_LIB=[ON|OFF]		use iconv as lib, OFF by default

	
	Optional Packages:
		-DSQLITE3_PATH:PATH=[DIR]		path to sqlite3 installation
		-DWITH_SQLITE3=[ON|OFF]			use sqlite3

		-DOPENSSL_PATH:PATH=[DIR]		path to openssl installation
		-DWITH_OPENSSL=[ON|OFF]			use openssl

		-DBFD_PATH:PATH=[DIR]			path to bfd installation
		-DWITH_BFD=[ON|OFF]			use bfd

		-DPOSTGRESQL_PATH:PATH=[DIR]		path to postgresql installation
		-DWITH_POSTGRESQL=[ON|OFF]		use postgresql

		-DMYSQL_PATH:PATH=[DIR]			path to mysql installation
		-DWITH_MYSQL=[ON|OFF]			use mysql

		-DFAST_CGI_PATH:PATH=[DIR]		path to fast-cgi installation
		-DWITH_FAST_CGI=[ON|OFF]			use fast-cgi

		-DPCRE_PATH:PATH=[DIR]			path to pcre installation
		-DWITH_PCRE=[ON|OFF]			use pcre

		-DDL_PATH:PATH=[DIR]			path to dl installation
		-DWITH_DL=[ON|OFF]			use dynamic linking

		-DLIBXML2_PATH:PATH=[DIR]		path to libxml2 installation
		-DWITH_LIBXML2=[ON|OFF]			use libxml2

		-DWITH_THREADING_LIB:STRING=[lib] 	threading library(supported:pthread). pthread by default
		-DPTHREAD_LIB:STRING=[lib]		use pthread library(supported:pthread, thr, lthread, c_r), pthread by default
		-DPTHREAD_PATH:PATH=[DIR]		path to pthread installation
		
		-DBZIP2_PATH:PATH=[DIR]			path to bzip2 installation
		-DWITH_BZIP2=[ON|OFF]			use bzip2

		-DZLIB_PATH:PATH=[DIR]			path to zlib installation
		-DWITH_ZLIB=[ON|OFF]			use zlib

		-DICONV_PATH:PATH=[DIR]			path to iconv installation
		-DWITH_ICONV=[ON|OFF]			use iconv
		
		-DIMAGEMAGICK_PATH:PATH=[DIR]		path to imagemagick installation
		-DWITH_IMAGEMAGICK=[ON|OFF]		use imagemagick
	")
	RETURN()
ENDIF(HELP)

##
#
##

ENABLE_LANGUAGE(CXX)

##
# functions
##

MACRO(GET_LIB_DIRS LDFLAGS LIB_DIRS)
	SET(${LIB_DIRS} "/usr/lib")
	STRING(REGEX MATCHALL "-L[^ ]*" MATCH_FLAG "${LDFLAGS}")
	FOREACH(LDFLAG ${MATCH_FLAG})
		STRING(REPLACE "-L" "" DIR ${LDFLAG})
		SET(${LIB_DIRS} ${${LIB_DIRS}} ${DIR})
	ENDFOREACH(LDFLAG)
ENDMACRO(GET_LIB_DIRS)

MACRO(GET_INCLUDE_DIRS CFLAGS INCLUDE_DIRS)
	SET(${INCLUDE_DIRS} "/usr/include")
	STRING(REGEX MATCHALL "-I[^ ]*" MATCH_FLAG "${CFLAGS}")
	FOREACH(CFLAG ${MATCH_FLAG})
		STRING(REPLACE "-I" "" DIR ${CFLAG})
		SET(${INCLUDE_DIRS} ${${INCLUDE_DIRS}} ${DIR})
	ENDFOREACH(CFLAG)
ENDMACRO(GET_INCLUDE_DIRS)

FUNCTION(ADD_LINK_DIRECTORIES)
	FOREACH(DIR ${ARGV})
		LINK_DIRECTORIES("${DIR}")
	ENDFOREACH(DIR)
ENDFUNCTION(ADD_LINK_DIRECTORIES)

FUNCTION(ADD_INCLUDE_DIRECTORIES)
	FOREACH(DIR ${ARGV})
		INCLUDE_DIRECTORIES("${DIR}")
	ENDFOREACH(DIR)
ENDFUNCTION(ADD_INCLUDE_DIRECTORIES)

##
# build options
##

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")

AUX_SOURCE_DIRECTORY(src library_sources)

ADD_LIBRARY(dodo SHARED ${library_sources})

TARGET_LINK_LIBRARIES(dodo rt)

ADD_CUSTOM_TARGET(lib DEPENDS dodo)
SET_TARGET_PROPERTIES(dodo PROPERTIES 
		VERSION ${PROJECT_VERSION} 
		SOVERSION ${PROJECT_VERSION}
		CLEAN_DIRECT_OUTPUT 1)

##

ADD_DEFINITIONS("-D_REENTRANT -D_GNU_SOURCE -D__USE_UNIX98 -D_XOPEN_SOURCE=600 -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_THREAD_SAFE")

##
# checking dependencies/build options
##

FILE(WRITE "include/libdodo/directives.runtime.h"
"/***************************************************************************
*            directives.runtime.h
****************************************************************************/

#ifndef _DIRECTIVES_RUNTIME_H_
#define _DIRECTIVES_RUNTIME_H_

")

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckCXXSourceCompiles)
INCLUDE(CheckLibraryExists)

OPTION(ENABLE_DEBUG "enable debug info(ON/OFF)" OFF)
IF(ENABLE_DEBUG)
	ADD_DEFINITIONS("-g -DDEBUG")
ENDIF(ENABLE_DEBUG)

OPTION(ENABLE_PROFILING "enable profiling(ON/OFF)" OFF)
IF(ENABLE_PROFILING)
	ADD_DEFINITIONS("-pg")
ENDIF(ENABLE_PROFILING)

OPTION(ENABLE_LIBWRAP "enable libwrap(ON/OFF)" ON)
IF(ENABLE_LIBWRAP)
	TARGET_LINK_LIBRARIES(dodo wrap)
ENDIF(ENABLE_LIBWRAP)

OPTION(ENABLE_DEQUE "use std::deque instead of std::vector(ON/OFF)" ON)
IF(ENABLE_DEQUE)
	FILE(APPEND "include/libdodo/directives.runtime.h" "#define USE_DEQUE\n\n")
ENDIF(ENABLE_DEQUE)

OPTION(ENABLE_IO_XEXEC "compile io:: with xexec support(ON/OFF)" ON)
IF(NOT ENABLE_IO_XEXEC)
	FILE(APPEND "include/libdodo/directives.runtime.h" "#define IO_WO_XEXEC\n\n")
ENDIF(NOT ENABLE_IO_XEXEC)

OPTION(ENABLE_DB_XEXEC "compile db:: with xexec support(ON/OFF)" ON)
IF(NOT ENABLE_DB_XEXEC)
	FILE(APPEND "include/libdodo/directives.runtime.h" "#define DATABASE_WO_XEXEC\n\n")
ENDIF(NOT ENABLE_DB_XEXEC)


SET(SQLITE3_PATH "/usr" CACHE PATH "installation path")
OPTION(WITH_SQLITE3 "use sqlite3(ON/OFF)" OFF)
IF(WITH_SQLITE3)
	FIND_PATH(SQLITE3_INCLUDE_DIR "sqlite3.h" "${SQLITE3_PATH}/include" "${SQLITE3_PATH}/local/include")
	CHECK_LIBRARY_EXISTS("sqlite3" "sqlite3_exec" "-L${SQLITE3_PATH}/lib" SQLITE3_LIB)

	IF(SQLITE3_INCLUDE_DIR AND SQLITE3_LIB)
		MESSAGE(STATUS "sqlite3 has been successfully found")

		INCLUDE_DIRECTORIES("${SQLITE3_INCLUDE_DIR}")
		LINK_DIRECTORIES("${SQLITE3_PATH}/lib")

		TARGET_LINK_LIBRARIES(dodo sqlite3)

		FILE(APPEND "include/libdodo/directives.runtime.h" "#define SQLITE3_EXT\n\n")
	ELSE(SQLITE3_INCLUDE_DIR AND SQLITE3_LIB)
		MESSAGE(FATAL_ERROR "Unable to find sqlite3")
	ENDIF(SQLITE3_INCLUDE_DIR AND SQLITE3_LIB)
ENDIF(WITH_SQLITE3)

SET(OPENSSL_PATH "/usr" CACHE PATH "installation path")
OPTION(WITH_OPENSSL "use openssl(ON/OFF)" OFF)
IF(WITH_OPENSSL)
	FIND_PATH(OPENSSL_INCLUDE_DIR "openssl/ssl.h" "${OPENSSL_PATH}/include" "${OPENSSL_PATH}/local/include")
	CHECK_LIBRARY_EXISTS("ssl" "SSL_library_init" "-L${OPENSSL_PATH}/lib" OPENSSL_LIB)

	IF(OPENSSL_INCLUDE_DIR AND OPENSSL_LIB)
		MESSAGE(STATUS "openssl has been successfully found")

		INCLUDE_DIRECTORIES("${OPENSSL_INCLUDE_DIR}")
		LINK_DIRECTORIES("${OPENSSL_PATH}/lib")

		TARGET_LINK_LIBRARIES(dodo ssl)

		FILE(APPEND "include/libdodo/directives.runtime.h" "#define OPENSSL_EXT\n\n")
	ELSE(OPENSSL_INCLUDE_DIR AND OPENSSL_LIB)
		MESSAGE(FATAL_ERROR "Unable to find openssl")
	ENDIF(OPENSSL_INCLUDE_DIR AND OPENSSL_LIB)
ENDIF(WITH_OPENSSL)

SET(BFD_PATH "/usr" CACHE PATH "installation path")
OPTION(WITH_BFD "use bfd(ON/OFF)" OFF)
IF(WITH_BFD)
	FIND_PATH(BFD_INCLUDE_DIR "bfd.h" "${BFD_PATH}/include" "${BFD_PATH}/local/include")
	CHECK_LIBRARY_EXISTS("bfd" "bfd_fopen" "-L${BFD_PATH}/lib" BFD_LIB)

	IF(BFD_INCLUDE_DIR AND BFD_LIB)
		MESSAGE(STATUS "bfd has been successfully found")

		INCLUDE_DIRECTORIES("${BFD_INCLUDE_DIR}")
		LINK_DIRECTORIES("${BFD_PATH}/lib")

		TARGET_LINK_LIBRARIES(dodo bfd)

		FILE(APPEND "include/libdodo/directives.runtime.h" "#define BFD_EXT\n\n")
	ELSE(BFD_INCLUDE_DIR AND BFD_LIB)
		MESSAGE(FATAL_ERROR "Unable to find bfd")
	ENDIF(BFD_INCLUDE_DIR AND BFD_LIB)
ENDIF(WITH_BFD)

SET(POSTGRESQL_PATH "/usr" CACHE PATH "installation path")
OPTION(WITH_POSTGRESQL "use postgresql(ON/OFF)" OFF)
IF(WITH_POSTGRESQL)
	FIND_PROGRAM(PG_CONFIG "pg_config" "/usr/bin" "/usr/local/bin")
	IF(PG_CONFIG)
		EXECUTE_PROCESS(COMMAND "pg_config" "--includedir" OUTPUT_VARIABLE POSTGRESQL_INCLUDE_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
		EXECUTE_PROCESS(COMMAND "pg_config" "--libdir" OUTPUT_VARIABLE POSTGRESQL_LIB_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
	ELSE(PG_CONFIG)
		SET(POSTGRESQL_INCLUDE_PATH "${POSTGRESQL_PATH}/include" "${POSTGRESQL_PATH}/include/postgresql" "${POSTGRESQL_PATH}/local/include" "${POSTGRESQL_PATH}/local/include/postgresql")
		SET(POSTGRESQL_LIB_PATH "${POSTGRESQL_PATH}/lib")
	ENDIF(PG_CONFIG)

	FIND_PATH(POSTGRESQL_INCLUDE_DIR "libpq-fe.h" ${POSTGRESQL_INCLUDE_PATH})
	CHECK_LIBRARY_EXISTS("pq" "PQclear" "-L${POSTGRESQL_LIB_PATH}" POSTGRESQL_LIB)

	IF(POSTGRESQL_INCLUDE_DIR AND POSTGRESQL_LIB)
		MESSAGE(STATUS "postresql has been successfully found")

		INCLUDE_DIRECTORIES("${POSTGRESQL_INCLUDE_DIR}")
		LINK_DIRECTORIES("${POSTGRESQL_LIB_PATH}")

		TARGET_LINK_LIBRARIES(dodo pq)

		FILE(APPEND "include/libdodo/directives.runtime.h" "#define POSTGRESQL_EXT\n\n")

		SET(CMAKE_REQUIRED_INCLUDES "${POSTGRESQL_INCLUDE_DIR}")
		SET(CMAKE_REQUIRED_LIBRARIES "pq")
		CHECK_CXX_SOURCE_COMPILES(
"
#include <libpq-fe.h>
int main(int argc, char **argv)
{
	pg_encoding_to_char(0);

	return 0;
}
"
		POSTGRESQL_HAVE_ENCODINGTOCHAR)
		SET(CMAKE_REQUIRED_INCLUDES)
		SET(CMAKE_REQUIRED_LIBRARIES)
		IF(NOT POSTGRESQL_HAVE_ENCODINGTOCHAR)
			FILE(APPEND "include/libdodo/directives.runtime.h" "#define POSTGRESQL_NO_ENCODINGTOCHAR\n\n")
		ENDIF(NOT POSTGRESQL_HAVE_ENCODINGTOCHAR)
	ELSE(POSTGRESQL_INCLUDE_DIR AND POSTGRESQL_LIB)
		MESSAGE(FATAL_ERROR "Unable to find postgresql")
	ENDIF(POSTGRESQL_INCLUDE_DIR AND POSTGRESQL_LIB)
ENDIF(WITH_POSTGRESQL)

SET(MYSQL_PATH "/usr" CACHE PATH "installation path")
OPTION(WITH_MYSQL "use mysql(ON/OFF)" OFF)
IF(WITH_MYSQL)
	FIND_PROGRAM(MYSQL_CONFIG "mysql_config" "/usr/bin" "/usr/local/bin")
	IF(MYSQL_CONFIG)
		EXECUTE_PROCESS(COMMAND "mysql_config" "--cflags" OUTPUT_VARIABLE MYSQL_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
		EXECUTE_PROCESS(COMMAND "mysql_config" "--libs" OUTPUT_VARIABLE	MYSQL_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
		GET_INCLUDE_DIRS("${MYSQL_CFLAGS}" MYSQL_INCLUDE_PATH)
		GET_LIB_DIRS("${MYSQL_LIBS}" MYSQL_LIB_PATH)
	ELSE(MYSQL_CONFIG)
		SET(MYSQL_INCLUDE_PATH "${MYSQL_PATH}/include" "${MYSQL_PATH}/include/mysql" "${MYSQL_PATH}/local/include" "${MYSQL_PATH}/local/include/mysql")
		SET(MYSQL_LIB_PATH "${MYSQL_PATH}/lib")
	ENDIF(MYSQL_CONFIG)

	FIND_PATH(MYSQL_INCLUDE_DIR "mysql.h" ${MYSQL_INCLUDE_PATH})
	FOREACH(FLAG ${MYSQL_LIB_PATH})
		SET(MYSQL_LDFLAGS "${MYSQL_LDFLAGS} -L${FLAG}")
	ENDFOREACH(FLAG)
	SET(CMAKE_REQUIRED_FLAGS ${MYSQL_LDFLAGS})
	CHECK_LIBRARY_EXISTS("mysqlclient" "mysql_init" "" MYSQL_LIB)
	SET(CMAKE_REQUIRED_FLAGS)

	IF(MYSQL_INCLUDE_DIR AND MYSQL_LIB)
		MESSAGE(STATUS "mysql has been successfully found")

		ADD_LINK_DIRECTORIES(${MYSQL_LIB_PATH})
		ADD_INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_PATH})

		TARGET_LINK_LIBRARIES(dodo mysqlclient)

		FILE(APPEND "include/libdodo/directives.runtime.h" "#define MYSQL_EXT\n\n")

		SET(CMAKE_REQUIRED_INCLUDES ${MYSQL_INCLUDE_PATH})
		SET(CMAKE_REQUIRED_LIBRARIES "mysqlclient")
		CHECK_CXX_SOURCE_COMPILES(
"
#include <mysql.h>
int main(int argc, char **argv)
{
	my_bool rc = 1;
	mysql_options(0, MYSQL_OPT_RECONNECT, &rc);

	return 0;
}
"
		MYSQL_HAVE_OPT_RECONNECT)
		SET(CMAKE_REQUIRED_INCLUDES)
		SET(CMAKE_REQUIRED_LIBRARIES)
		IF(NOT MYSQL_HAVE_OPT_RECONNECT)
			FILE(APPEND "include/libdodo/directives.runtime.h" "#define MYSQL_NO_OPT_RECONNECT\n\n")
		ENDIF(NOT MYSQL_HAVE_OPT_RECONNECT)
	ELSE(MYSQL_INCLUDE_DIR AND MYSQL_LIB)
		MESSAGE(FATAL_ERROR "Unable to find mysql")
	ENDIF(MYSQL_INCLUDE_DIR AND MYSQL_LIB)
ENDIF(WITH_MYSQL)

SET(FAST_CGI_PATH "/usr" CACHE PATH "installation path")
OPTION(WITH_FAST_CGI "use fast-cgi(ON/OFF)" OFF)
IF(WITH_FAST_CGI)
	FIND_PATH(FAST_CGI_INCLUDE_DIR "fcgiapp.h" "${FAST_CGI_PATH}/include" "${FAST_CGI_PATH}/local/include")
	CHECK_LIBRARY_EXISTS("fcgi" "FCGX_Init" "-L${FAST_CGI_PATH}/lib" FAST_CGI_LIB)

	IF(FAST_CGI_INCLUDE_DIR AND FAST_CGI_LIB)
		MESSAGE(STATUS "fast-cgi has been successfully found")

		INCLUDE_DIRECTORIES("${FAST_CGI_INCLUDE_DIR}")
		LINK_DIRECTORIES("${FAST_CGI_PATH}/lib")

		TARGET_LINK_LIBRARIES(dodo fcgi)

		FILE(APPEND "include/libdodo/directives.runtime.h" "#define FAST_CGI_EXT\n\n")
	ELSE(FAST_CGI_INCLUDE_DIR AND FAST_CGI_LIB)
		MESSAGE(FATAL_ERROR "Unable to find fast-cgi")
	ENDIF(FAST_CGI_INCLUDE_DIR AND FAST_CGI_LIB)
ENDIF(WITH_FAST_CGI)

SET(PCRE_PATH "/usr" CACHE PATH "installation path")
OPTION(WITH_PCRE "use pcre(ON/OFF)" OFF)
IF(WITH_PCRE)
	FIND_PROGRAM(PCRE_CONFIG "pcre-config" "/usr/bin" "/usr/local/bin")
	IF(PCRE_CONFIG)
		EXECUTE_PROCESS(COMMAND "pcre-config" "--cflags" OUTPUT_VARIABLE PCRE_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
		EXECUTE_PROCESS(COMMAND "pcre-config" "--libs" OUTPUT_VARIABLE	PCRE_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
		GET_INCLUDE_DIRS("${PCRE_CFLAGS}" PCRE_INCLUDE_PATH)
		GET_LIB_DIRS("${PCRE_LIBS}" PCRE_LIB_PATH)
	ELSE(PCRE_CONFIG)
		SET(PCRE_INCLUDE_PATH "${PCRE_PATH}/include" "${PCRE_PATH}/local/include")
		SET(PCRE_LIB_PATH "${PCRE_PATH}/lib")
	ENDIF(PCRE_CONFIG)

	FIND_PATH(PCRE_INCLUDE_DIR "pcre.h" ${PCRE_INCLUDE_PATH})
	FOREACH(FLAG ${PCRE_LIB_PATH})
		SET(PCRE_LDFLAGS "${PCRE_LDFLAGS} -L${FLAG}")
	ENDFOREACH(FLAG)
	SET(CMAKE_REQUIRED_FLAGS ${PCRE_LDFLAGS})
	CHECK_LIBRARY_EXISTS("pcre" "pcre_exec" "" PCRE_LIB)
	SET(CMAKE_REQUIRED_FLAGS)

	IF(PCRE_INCLUDE_DIR AND PCRE_LIB)
		MESSAGE(STATUS "pcre has been successfully found")

		ADD_INCLUDE_DIRECTORIES(${PCRE_INCLUDE_PATH})
		ADD_LINK_DIRECTORIES(${PCRE_LIB_PATH})

		TARGET_LINK_LIBRARIES(dodo pcre)

		FILE(APPEND "include/libdodo/directives.runtime.h" "#define PCRE_EXT\n\n")
	ELSE(PCRE_INCLUDE_DIR AND PCRE_LIB)
		MESSAGE(FATAL_ERROR "Unable to find pcre")
	ENDIF(PCRE_INCLUDE_DIR AND PCRE_LIB)
ELSE(WITH_PCRE)
	FIND_PATH(POSIXRE_INCLUDE_DIR "regex.h" "${PCRE_PATH}/include" "${PCRE_PATH}/local/include")
	IF(POSIXRE_INCLUDE_DIR)
		MESSAGE(STATUS "posix regex has been successfully found")
	ELSE(POSIXRE_INCLUDE_DIR)
		MESSAGE(FATAL_ERROR "Unable to find posix regex")
	ENDIF(POSIXRE_INCLUDE_DIR)
ENDIF(WITH_PCRE)

SET(DL_PATH "/usr" CACHE PATH "installation path")
OPTION(WITH_DL "use dynamic linking(ON/OFF)" OFF)
IF(WITH_DL)
	FIND_PATH(DL_INCLUDE_DIR "dlfcn.h" "${DL_PATH}/include" "${DL_PATH}/local/include")

	OPTION(ENABLE_DL_LIB "use dl as lib(ON/OFF)" ON)
	IF(ENABLE_DL_LIB)
		CHECK_LIBRARY_EXISTS("dl" "dlopen" "-L${DL_PATH}/lib" DL_LIB)
		IF(DL_LIB)
			MESSAGE(STATUS "dl lib has been successfully found")
		ELSE(DL_LIB)
			MESSAGE(STATUS "unable to found dl lib")
		ENDIF(DL_LIB)
	ENDIF(ENABLE_DL_LIB)

	OPTION(ENABLE_FAST_DYNAMIC_LINKING "Do not unload library to produce better performance(ON/OFF)" ON)

	IF(DL_INCLUDE_DIR)
		MESSAGE(STATUS "dl has been successfully found")

		INCLUDE_DIRECTORIES("${DL_INCLUDE_DIR}")

		IF(ENABLE_DL_LIB)
			LINK_DIRECTORIES("${DL_PATH}/lib")

			TARGET_LINK_LIBRARIES(dodo dl)
		ENDIF(ENABLE_DL_LIB)

		FILE(APPEND "include/libdodo/directives.runtime.h" "#define DL_EXT\n\n")

		IF(ENABLE_FAST_DYNAMIC_LINKING)
			FILE(APPEND "include/libdodo/directives.runtime.h" "#define DL_FAST\n\n")
		ENDIF(ENABLE_FAST_DYNAMIC_LINKING)
	ELSE(DL_INCLUDE_DIR AND DL_LIB)
		MESSAGE(FATAL_ERROR "Unable to find dl")
	ENDIF(DL_INCLUDE_DIR)
ENDIF(WITH_DL)

SET(LIBXML2_PATH "/usr" CACHE PATH "installation path")
OPTION(WITH_LIBXML2 "use libxml2(ON/OFF)" OFF)
IF(WITH_LIBXML2)
	FIND_PROGRAM(LIBXML2_CONFIG "xml2-config" "/usr/bin" "/usr/local/bin")
	IF(LIBXML2_CONFIG)
		EXECUTE_PROCESS(COMMAND "xml2-config" "--cflags" OUTPUT_VARIABLE LIBXML2_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
		EXECUTE_PROCESS(COMMAND "xml2-config" "--libs" OUTPUT_VARIABLE	LIBXML2_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
		GET_INCLUDE_DIRS("${LIBXML2_CFLAGS}" LIBXML2_INCLUDE_PATH)
		GET_LIB_DIRS("${LIBXML2_LIBS}" LIBXML2_LIB_PATH)
	ELSE(LIBXML2_CONFIG)
		SET(LIBXML2_INCLUDE_PATH "${LIBXML2_PATH}/include" "${LIBXML2_PATH}/include/libxml2" "${LIBXML2_PATH}/local/include" "${LIBXML2_PATH}/local/include/libxml2")
		SET(LIBXML2_LIB_PATH "${LIBXML2_PATH}/lib")
	ENDIF(LIBXML2_CONFIG)

	FIND_PATH(LIBXML2_INCLUDE_DIR "libxml/xmlmemory.h" ${LIBXML2_INCLUDE_PATH})
	FOREACH(FLAG ${LIBXML2_LIB_PATH})
		SET(LIBXML2_LDFLAGS "${LIBXML2_LDFLAGS} -L${FLAG}")
	ENDFOREACH(FLAG)
	SET(CMAKE_REQUIRED_FLAGS ${LIBXML2_LDFLAGS})
	CHECK_LIBRARY_EXISTS("xml2" "xmlInitParser" "" LIBXML2_LIB)
	SET(CMAKE_REQUIRED_FLAGS)

	IF(LIBXML2_INCLUDE_DIR AND LIBXML2_LIB)
		MESSAGE(STATUS "libxml2 has been successfully found")

		ADD_LINK_DIRECTORIES(${LIBXML2_LIB_PATH})
		ADD_INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_PATH})

		TARGET_LINK_LIBRARIES(dodo xml2)

		FILE(APPEND "include/libdodo/directives.runtime.h" "#define LIBXML2_EXT\n\n")
	ELSE(LIBXML2_INCLUDE_DIR AND LIBXML2_LIB)
		MESSAGE(FATAL_ERROR "Unable to find libxml2")
	ENDIF(LIBXML2_INCLUDE_DIR AND LIBXML2_LIB)
ENDIF(WITH_LIBXML2)

SET(WITH_THREADING_LIB "pthread" CACHE STRING "threading library(supported:pthread; default=pthread)")
IF(WITH_THREADING_LIB STREQUAL "pthread")
	SET(PTHREAD_LIB "pthread" CACHE STRING "use pthread library(supported:pthread, thr, lthread, c_r) (default=pthread)")
	SET(PTHREAD_PATH "/usr" CACHE PATH "installation path")

	FIND_PATH(THREADING_LIB_INCLUDE_DIR "pthread.h" "${PTHREAD_PATH}/include" "${PTHREAD_PATH}/local/include")
	CHECK_LIBRARY_EXISTS("${PTHREAD_LIB}" "pthread_create" "-L${THREADING_LIB_PATH}/lib" THREADING_LIB_LIB)

	IF(THREADING_LIB_INCLUDE_DIR AND THREADING_LIB_LIB)
		MESSAGE(STATUS "pthread has been successfully found")

		INCLUDE_DIRECTORIES("${THREADING_LIB_INCLUDE_DIR}")
		LINK_DIRECTORIES("${PTHREAD_PATH}/lib")

		TARGET_LINK_LIBRARIES(dodo "${PTHREAD_LIB}")

		FILE(APPEND "include/libdodo/directives.runtime.h" "#define PTHREAD_EXT\n\n")
	ELSE(THREADING_LIB_INCLUDE_DIR AND THREADING_LIB_LIB)
		MESSAGE(FATAL_ERROR "Unable to find pthread")
	ENDIF(THREADING_LIB_INCLUDE_DIR AND THREADING_LIB_LIB)
#ELIF(WITH_THREADING_LIB EQUAL "")
#
ELSE(WITH_THREADING_LIB STREQUAL "pthread")
	MESSAGE(FATAL_ERROR "Unsuported threading library specified")
ENDIF(WITH_THREADING_LIB STREQUAL "pthread")

SET(BZIP2_PATH "/usr" CACHE PATH "installation path")
OPTION(WITH_BZIP2 "use bzip2(ON/OFF)" OFF)
IF(WITH_BZIP2)
	FIND_PATH(BZIP2_INCLUDE_DIR "bzlib.h" "${BZIP2_PATH}/include" "${BZIP2_PATH}/local/include")
	CHECK_LIBRARY_EXISTS("bz2" "BZ2_bzBuffToBuffCompress" "-L${BZIP2_PATH}/lib" BZIP2_LIB)

	IF(BZIP2_INCLUDE_DIR AND BZIP2_LIB)
		MESSAGE(STATUS "bz2 has been successfully found")

		INCLUDE_DIRECTORIES("${BZIP2_INCLUDE_DIR}")
		LINK_DIRECTORIES("${BZIP2_PATH}/lib")

		TARGET_LINK_LIBRARIES(dodo bz2)

		FILE(APPEND "include/libdodo/directives.runtime.h" "#define BZIP2_EXT\n\n")
	ELSE(BZIP2_INCLUDE_DIR AND BZIP2_LIB)
		MESSAGE(FATAL_ERROR "Unable to find bz2")
	ENDIF(BZIP2_INCLUDE_DIR AND BZIP2_LIB)
ENDIF(WITH_BZIP2)

SET(ZLIB_PATH "/usr" CACHE PATH "installation path")
OPTION(WITH_ZLIB "use zlib(ON/OFF)" OFF)
IF(WITH_ZLIB)
	FIND_PATH(ZLIB_INCLUDE_DIR "zlib.h" "${ZLIB_PATH}/include" "${ZLIB_PATH}/local/include")

	OPTION(ENABLE_ZLIB_LIB "use zlib as lib(ON/OFF)" OFF)
	IF(ENABLE_ZLIB_LIB)
		CHECK_LIBRARY_EXISTS("z" "deflateInit2" "-L${ZLIB_PATH}/lib" ZLIB_LIB)
		IF(ZLIB_LIB)
			MESSAGE(STATUS "zlib lib has been successfully found")
		ELSE(ZLIB_LIB)
			MESSAGE(STATUS "unable to found zlib lib")
		ENDIF(ZLIB_LIB)
	ENDIF(ENABLE_ZLIB_LIB)

	IF(ZLIB_INCLUDE_DIR)
		MESSAGE(STATUS "zlib has been successfully found")

		INCLUDE_DIRECTORIES("${ZLIB_INCLUDE_DIR}")

		IF(ENABLE_ZLIB_LIB)
			LINK_DIRECTORIES("${ZLIB_PATH}/lib")

			TARGET_LINK_LIBRARIES(dodo z)
		ENDIF(ENABLE_ZLIB_LIB)

		FILE(APPEND "include/libdodo/directives.runtime.h" "#define ZLIB_EXT\n\n")
	ELSE(ZLIB_INCLUDE_DIR AND ZLIB_LIB)
		MESSAGE(FATAL_ERROR "Unable to find zlib")
	ENDIF(ZLIB_INCLUDE_DIR)
ENDIF(WITH_ZLIB)

SET(ICONV_PATH "/usr" CACHE PATH "installation path")
OPTION(WITH_ICONV "use iconv(ON/OFF)" OFF)
IF(WITH_ICONV)
	FIND_PATH(ICONV_INCLUDE_DIR "iconv.h" "${ICONV_PATH}/include" "${ICONV_PATH}/local/include")

	OPTION(ENABLE_ICONV_LIB "use iconv as lib(ON/OFF)" OFF)
	IF(ENABLE_ICONV_LIB)
		CHECK_LIBRARY_EXISTS("iconv" "iconv" "-L${ICONV_PATH}/lib" ICONV_LIB)
		IF(ICONV_LIB)
			MESSAGE(STATUS "iconv lib has been successfully found")
		ELSE(ICONV_LIB)
			MESSAGE(STATUS "unable to found iconv lib")
		ENDIF(ICONV_LIB)
	ENDIF(ENABLE_ICONV_LIB)

	IF(ICONV_INCLUDE_DIR)
		MESSAGE(STATUS "iconv has been successfully found")

		INCLUDE_DIRECTORIES("${ICONV_INCLUDE_DIR}")

		IF(ENABLE_ICONV_LIB)
			LINK_DIRECTORIES("${ICONV_PATH}/lib")

			TARGET_LINK_LIBRARIES(dodo iconv)
		ENDIF(ENABLE_ICONV_LIB)

		FILE(APPEND "include/libdodo/directives.runtime.h" "#define ICONV_EXT\n\n")
	ELSE(ICONV_INCLUDE_DIR AND ICONV_LIB)
		MESSAGE(FATAL_ERROR "Unable to find iconv")
	ENDIF(ICONV_INCLUDE_DIR)
ENDIF(WITH_ICONV)

SET(IMAGEMAGICK_PATH "/usr" CACHE PATH "installation path")
OPTION(WITH_IMAGEMAGICK "use imagemagick(ON/OFF)" OFF)
IF(WITH_IMAGEMAGICK)
	FIND_PROGRAM(IMAGEMAGICK_CONFIG "Magick-config" "/usr/bin" "/usr/local/bin")
	IF(IMAGEMAGICK_CONFIG)
		EXECUTE_PROCESS(COMMAND "Magick-config" "--cppflags" OUTPUT_VARIABLE IMAGEMAGICK_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
		EXECUTE_PROCESS(COMMAND "Magick-config" "--ldflags" OUTPUT_VARIABLE	IMAGEMAGICK_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
		GET_INCLUDE_DIRS("${IMAGEMAGICK_CFLAGS}" IMAGEMAGICK_INCLUDE_PATH)
		GET_LIB_DIRS("${IMAGEMAGICK_LIBS}" IMAGEMAGICK_LIB_PATH)
	ELSE(IMAGEMAGICK_CONFIG)
		SET(IMAGEMAGICK_INCLUDE_PATH "${IMAGEMAGICK_PATH}/include" "${IMAGEMAGICK_PATH}/include/ImageMagick" "${IMAGEMAGICK_PATH}/local/include" "${IMAGEMAGICK_PATH}/local/include/ImageMagick")
		SET(IMAGEMAGICK_LIB_PATH "${IMAGEMAGICK_PATH}/lib")
	ENDIF(IMAGEMAGICK_CONFIG)

	FIND_PATH(IMAGEMAGICK_INCLUDE_DIR "magick/MagickCore.h" ${IMAGEMAGICK_INCLUDE_PATH})
	FOREACH(FLAG ${IMAGEMAGICK_LIB_PATH})
		SET(IMAGEMAGICK_LDFLAGS "${IMAGEMAGICK_LDFLAGS} -L${FLAG}")
	ENDFOREACH(FLAG)
	SET(CMAKE_REQUIRED_FLAGS ${IMAGEMAGICK_LDFLAGS})
	CHECK_LIBRARY_EXISTS("MagickCore" "MagickCoreGenesis" "" IMAGEMAGICK_LIB)
	IF(NOT IMAGEMAGICK_LIB)
		CHECK_LIBRARY_EXISTS("Magick" "MagickCoreGenesis" "" OLD_IMAGEMAGICK_LIB)
		SET(IMAGEMAGICK_LIB TRUE)
	ENDIF(NOT IMAGEMAGICK_LIB)
	SET(CMAKE_REQUIRED_FLAGS)

	IF(IMAGEMAGICK_INCLUDE_DIR AND IMAGEMAGICK_LIB)
		MESSAGE(STATUS "imagemagick has been successfully found")

		ADD_LINK_DIRECTORIES(${IMAGEMAGICK_LIB_PATH})
		ADD_INCLUDE_DIRECTORIES(${IMAGEMAGICK_INCLUDE_PATH})

		IF(NOT OLD_IMAGEMAGICK_LIB)
			TARGET_LINK_LIBRARIES(dodo MagickCore)
		ELSE(NOT OLD_IMAGEMAGICK_LIB)
			TARGET_LINK_LIBRARIES(dodo Magick)
		ENDIF(NOT OLD_IMAGEMAGICK_LIB)

		FILE(APPEND "include/libdodo/directives.runtime.h" "#define IMAGEMAGICK_EXT\n\n")

		OPTION(ENABLE_GRAPHICS_XEXEC "compile graphics:: with xexec support(ON/OFF)" ON)
		IF(NOT ENABLE_GRAPHICS_XEXEC)
			FILE(APPEND "include/libdodo/directives.runtime.h" "#define GRAPHICS_WO_XEXEC\n\n")
		ENDIF(NOT ENABLE_GRAPHICS_XEXEC)
	ELSE(IMAGEMAGICK_INCLUDE_DIR AND IMAGEMAGICK_LIB)
		MESSAGE(FATAL_ERROR "Unable to find imagemagick")
	ENDIF(IMAGEMAGICK_INCLUDE_DIR AND IMAGEMAGICK_LIB)
ENDIF(WITH_IMAGEMAGICK)

CHECK_INCLUDE_FILES("arpa/inet.h" HAVE_ARPA_INET_H)
IF(HAVE_ARPA_INET_H)
	ADD_DEFINITIONS("-DHAVE_ARPA_INET_H")
ENDIF(HAVE_ARPA_INET_H)

CHECK_INCLUDE_FILES("fcntl.h" HAVE_FCNTL_H)
IF(HAVE_FCNTL_H)
	ADD_DEFINITIONS("-DHAVE_FCNTL_H")
ENDIF(HAVE_FCNTL_H)

CHECK_INCLUDE_FILES("netdb.h" HAVE_NETDB_H)
IF(HAVE_NETDB_H)
	ADD_DEFINITIONS("-DHAVE_NETDB_H")
ENDIF(HAVE_NETDB_H)

CHECK_INCLUDE_FILES("stdlib.h" HAVE_STDLIB_H)
IF(HAVE_STDLIB_H)
	ADD_DEFINITIONS("-DHAVE_STDLIB_H")
ENDIF(HAVE_STDLIB_H)

CHECK_INCLUDE_FILES("string.h" HAVE_STRING_H)
IF(HAVE_STRING_H)
	ADD_DEFINITIONS("-DHAVE_STRING_H")
ENDIF(HAVE_STRING_H)

CHECK_INCLUDE_FILES("unistd.h" HAVE_UNISTD_H)
IF(HAVE_UNISTD_H)
	ADD_DEFINITIONS("-DHAVE_UNISTD_H")
ENDIF(HAVE_UNISTD_H)

CHECK_INCLUDE_FILES("utime.h" HAVE_UTIME_H)
IF(HAVE_UTIME_H)
	ADD_DEFINITIONS("-DHAVE_UTIME_H")
ENDIF(HAVE_UTIME_H)

CHECK_INCLUDE_FILES("poll.h" HAVE_POLL_H)
IF(HAVE_POLL_H)
	ADD_DEFINITIONS("-DHAVE_POLL_H")
ENDIF(HAVE_POLL_H)

CHECK_INCLUDE_FILES("ctype.h" HAVE_CTYPE_H)
IF(HAVE_CTYPE_H)
	ADD_DEFINITIONS("-DHAVE_CTYPE_H")
ENDIF(HAVE_CTYPE_H)

CHECK_INCLUDE_FILES("errno.h" HAVE_ERRNO_H)
IF(HAVE_ERRNO_H)
	ADD_DEFINITIONS("-DHAVE_ERRNO_H")
ENDIF(HAVE_ERRNO_H)

CHECK_INCLUDE_FILES("grp.h" HAVE_GRP_H)
IF(HAVE_GRP_H)
	ADD_DEFINITIONS("-DHAVE_GRP_H")
ENDIF(HAVE_GRP_H)

CHECK_INCLUDE_FILES("libgen.h" HAVE_LIBGEN_H)
IF(HAVE_LIBGEN_H)
	ADD_DEFINITIONS("-DHAVE_LIBGEN_H")
ENDIF(HAVE_LIBGEN_H)

CHECK_INCLUDE_FILES("math.h" HAVE_MATH_H)
IF(HAVE_MATH_H)
	ADD_DEFINITIONS("-DHAVE_MATH_H")
ENDIF(HAVE_MATH_H)

CHECK_INCLUDE_FILES("net/if.h" HAVE_NET_IF_H)
IF(HAVE_NET_IF_H)
	ADD_DEFINITIONS("-DHAVE_NET_IF_H")
ENDIF(HAVE_NET_IF_H)

CHECK_INCLUDE_FILES("netinet/in.h" HAVE_NETINET_IN_H)
IF(HAVE_NETINET_IN_H)
	ADD_DEFINITIONS("-DHAVE_NETINET_IN_H")
ENDIF(HAVE_NETINET_IN_H)

CHECK_INCLUDE_FILES("pwd.h" HAVE_PWD_H)
IF(HAVE_PWD_H)
	ADD_DEFINITIONS("-DHAVE_PWD_H")
ENDIF(HAVE_PWD_H)

CHECK_INCLUDE_FILES("semaphore.h" HAVE_SEMAPHORE_H)
IF(HAVE_SEMAPHORE_H)
	ADD_DEFINITIONS("-DHAVE_SEMAPHORE_H")
ENDIF(HAVE_SEMAPHORE_H)

CHECK_INCLUDE_FILES("signal.h" HAVE_SIGNAL_H)
IF(HAVE_SIGNAL_H)
	ADD_DEFINITIONS("-DHAVE_SIGNAL_H")
ENDIF(HAVE_SIGNAL_H)

CHECK_INCLUDE_FILES("stdarg.h" HAVE_STDARG_H)
IF(HAVE_STDARG_H)
	ADD_DEFINITIONS("-DHAVE_STDARG_H")
ENDIF(HAVE_STDARG_H)

CHECK_INCLUDE_FILES("stdio.h" HAVE_STDIO_H)
IF(HAVE_STDIO_H)
	ADD_DEFINITIONS("-DHAVE_STDIO_H")
ENDIF(HAVE_STDIO_H)

CHECK_INCLUDE_FILES("stdlib.h" HAVE_STDLIB_H)
IF(HAVE_STDLIB_H)
	ADD_DEFINITIONS("-DHAVE_STDLIB_H")
ENDIF(HAVE_STDLIB_H)

CHECK_INCLUDE_FILES("syslog.h" HAVE_SYSLOG_H)
IF(HAVE_SYSLOG_H)
	ADD_DEFINITIONS("-DHAVE_SYSLOG_H")
ENDIF(HAVE_SYSLOG_H)

CHECK_INCLUDE_FILES("sys/ioctl.h" HAVE_SYS_IOCTL_H)
IF(HAVE_SYS_IOCTL_H)
	ADD_DEFINITIONS("-DHAVE_SYS_IOCTL_H")
ENDIF(HAVE_SYS_IOCTL_H)

CHECK_INCLUDE_FILES("symman.h" HAVE_SYMMAN_H)
IF(HAVE_SYMMAN_H)
	ADD_DEFINITIONS("-DHAVE_SYMMAN_H")
ENDIF(HAVE_SYMMAN_H)

CHECK_INCLUDE_FILES("sys/resource.h" HAVE_SYS_RESOURCE_H)
IF(HAVE_SYS_RESOURCE_H)
	ADD_DEFINITIONS("-DHAVE_SYS_RESOURCE_H")
ENDIF(HAVE_SYS_RESOURCE_H)

CHECK_INCLUDE_FILES("sys/socket.h" HAVE_SYS_SOCKET_H)
IF(HAVE_SYS_SOCKET_H)
	ADD_DEFINITIONS("-DHAVE_SYS_SOCKET_H")
ENDIF(HAVE_SYS_SOCKET_H)

CHECK_INCLUDE_FILES("sys/stat" HAVE_SYS_STAT)
IF(HAVE_SYS_STAT)
	ADD_DEFINITIONS("-DHAVE_SYS_STAT")
ENDIF(HAVE_SYS_STAT)

CHECK_INCLUDE_FILES("sys/time.h" HAVE_SYS_TIME_H)
IF(HAVE_SYS_TIME_H)
	ADD_DEFINITIONS("-DHAVE_SYS_TIME_H")
ENDIF(HAVE_SYS_TIME_H)

CHECK_INCLUDE_FILES("sys/types.h" HAVE_SYS_TYPES_H)
IF(HAVE_SYS_TYPES_H)
	ADD_DEFINITIONS("-DHAVE_SYS_TYPES_H")
ENDIF(HAVE_SYS_TYPES_H)

CHECK_INCLUDE_FILES("sys/un.h" HAVE_SYS_UN_H)
IF(HAVE_SYS_UN_H)
	ADD_DEFINITIONS("-DHAVE_SYS_UN_H")
ENDIF(HAVE_SYS_UN_H)

CHECK_INCLUDE_FILES("sys/wait" HAVE_SYS_WAIT)
IF(HAVE_SYS_WAIT)
	ADD_DEFINITIONS("-DHAVE_SYS_WAIT")
ENDIF(HAVE_SYS_WAIT)

CHECK_INCLUDE_FILES("dirent.h" HAVE_DIRENT_H)
IF(HAVE_DIRENT_H)
	ADD_DEFINITIONS("-DHAVE_DIRENT_H")
ENDIF(HAVE_DIRENT_H)

CHECK_INCLUDE_FILES("unistd.h" HAVE_UNISTD_H)
IF(HAVE_UNISTD_H)
	ADD_DEFINITIONS("-DHAVE_UNISTD_H")
ENDIF(HAVE_UNISTD_H)

CHECK_INCLUDE_FILES("utime.h" HAVE_UTIME_H)
IF(HAVE_UTIME_H)
	ADD_DEFINITIONS("-DHAVE_UTIME_H")
ENDIF(HAVE_UTIME_H)

CHECK_INCLUDE_FILES("time.h" HAVE_TIME_H)
IF(HAVE_TIME_H)
	ADD_DEFINITIONS("-DHAVE_TIME_H")
ENDIF(HAVE_TIME_H)

CHECK_INCLUDE_FILES("netdb.h" HAVE_NETDB_H)
IF(HAVE_NETDB_H)
	ADD_DEFINITIONS("-DHAVE_NETDB_H")
ENDIF(HAVE_NETDB_H)

FILE(APPEND "include/libdodo/directives.runtime.h" "#endif\n")

SET(prefix ${CMAKE_INSTALL_PREFIX})
SET(exec_prefix "${prefix}/bin")
SET(includedir "${prefix}/include")
SET(libdir "${prefix}/lib")
SET(CXX ${CMAKE_CXX_COMPILER})

GET_DIRECTORY_PROPERTY(_CPPFLAGS_ INCLUDE_DIRECTORIES)
FOREACH(FLAG ${_CPPFLAGS_})
	SET(CPPFLAGS "${CPPFLAGS} -I${FLAG}")
ENDFOREACH(FLAG)

GET_DIRECTORY_PROPERTY(CFLAGS DEFINITIONS)
STRING(REPLACE "\"${PROJECT_NAME}\"" "\\\"${PROJECT_NAME}\\\"" CFLAGS ${CFLAGS})
STRING(REPLACE "\"${PROJECT_VERSION}\"" "\\\"${PROJECT_NAME}\\\"" CFLAGS ${CFLAGS})
STRING(REPLACE "\"${PACKAGE_STRING}\"" "\\\"${PROJECT_NAME}\\ ${PROJECT_VERSION}\\\"" CFLAGS ${CFLAGS})

GET_DIRECTORY_PROPERTY(_LDFLAGS_ LINK_DIRECTORIES)
FOREACH(FLAG ${_LDFLAGS_})
	SET(LDFLAGS "${LDFLAGS} -L${FLAG}")
ENDFOREACH(FLAG)

CONFIGURE_FILE("bin/dodo-config.in" "bin/dodo-config" @ONLY)
EXECUTE_PROCESS(COMMAND "chmod" "+x" "bin/dodo-config")

CONFIGURE_FILE("tests/Makefile.in" "tests/Makefile" @ONLY)
CONFIGURE_FILE("tests/image_test/Makefile.in" "tests/image_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/cgifast_test/Makefile.in" "tests/cgifast_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/cgi_test/Makefile.in" "tests/cgi_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/dbmysql_test/Makefile.in" "tests/dbmysql_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/dbpostgresql_test/Makefile.in" "tests/dbpostgresql_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/dbsqlite_test/Makefile.in" "tests/dbsqlite_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/db_test/Makefile.in" "tests/db_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/iononblocked_ionetwork_test/Makefile.in" "tests/iononblocked_ionetwork_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/iononblocked_ionetworkssl_test/Makefile.in" "tests/iononblocked_ionetworkssl_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/iostdio_test/Makefile.in" "tests/iostdio_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/iofile_toolsfilesystem_test/Makefile.in" "tests/iofile_toolsfilesystem_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/iopipe_iofilefifo_pcthreadcollection_test/Makefile.in" "tests/iopipe_iofilefifo_pcthreadcollection_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/ionetwork_pcthreads_test/Makefile.in" "tests/ionetwork_pcthreads_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/ionetworkclient_test/Makefile.in" "tests/ionetworkclient_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/ionetworkserver_test/Makefile.in" "tests/ionetworkserver_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/ionetworksslserver_test/Makefile.in" "tests/ionetworksslserver_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/ionetworksslclient_test/Makefile.in" "tests/ionetworksslclient_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/ionetworkhttp_test/Makefile.in" "tests/ionetworkhttp_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/pcthreadcollection_test/Makefile.in" "tests/pcthreadcollection_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/pcprocesscollection_test/Makefile.in" "tests/pcprocesscollection_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/pcjobcollection_test/Makefile.in" "tests/pcjobcollection_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/toolsnetwork_test/Makefile.in" "tests/toolsnetwork_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/toolsos_test/Makefile.in" "tests/toolsos_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/toolstime_test/Makefile.in" "tests/toolstime_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/toolsmisc_test/Makefile.in" "tests/toolsmisc_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/toolscode_test/Makefile.in" "tests/toolscode_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/toolslogger_test/Makefile.in" "tests/toolslogger_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/toolslibrary_test/Makefile.in" "tests/toolslibrary_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/toolsregexp_test/Makefile.in" "tests/toolsregexp_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/xexec_test/Makefile.in" "tests/xexec_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/rpcxmlhttpclient_test/Makefile.in" "tests/rpcxmlhttpclient_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/rpcxmlcgiserver_test/Makefile.in" "tests/rpcxmlcgiserver_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/rpcjsonhttpclient_test/Makefile.in" "tests/rpcjsonhttpclient_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/rpcjsoncgiserver_test/Makefile.in" "tests/rpcjsoncgiserver_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/dataformatjson_test/Makefile.in" "tests/dataformatjson_test/Makefile" @ONLY)
CONFIGURE_FILE("tests/dataformatxml_test/Makefile.in" "tests/dataformatxml_test/Makefile" @ONLY)

##
# install options
##

INSTALL(DIRECTORY "include/libdodo" DESTINATION include)
INSTALL(PROGRAMS "bin/dodo-config" DESTINATION bin)
INSTALL(FILES "libdodo.so.${PROJECT_VERSION}" DESTINATION lib)
INSTALL(FILES "libdodo.so" DESTINATION lib)
