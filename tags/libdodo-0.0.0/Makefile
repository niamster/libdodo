LIBTOOL = libtool
CXX = g++
CFLAGS=-O3 -fmove-all-movables -march=pentium4
OBJECTS=sqlBaseEx.o sqlBase.o baseEx.o mysqlinterface.o mysqlinterfaceEx.o

override DEFINES:=$(DEFINES)

override CPPFLAGS:=-I/opt/mysql/include/mysql/ $(CPPFLAGS)
override LDFLAGS:=-L/opt/mysql/lib/mysql -L./ $(LDFLAGS)

override LIBS:=-lmysqlclient $(LIBS)

DODO_INCLUDE=-I./ 
DODO_LIBRARY=-L./

VERSION = 0

MINOR = 1

dodo: $(OBJECTS)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) $(LIBS) -fPIC -shared -W1,-soname,libdodo.so -o lib$@.so.$(VERSION).$(MINOR) $^
	#$(LIBTOOL) --mode=link $(CXX) -all-static $(LDFLAGS) $(LIBS) -fPIC -shared -W1,-soname,libdodo.so -o lib$@.so.$(VERSION).$(MINOR) $^
	ln -fs lib$@.so.$(VERSION).$(MINOR) lib$@.so
	ldconfig -n ./
.cc.o:
	$(LIBTOOL) --mode=compile $(CXX) $(CPPFLAGS) $(CFLAGS) -fPIC -c $^
	strip -d $@

clean:
	rm -rf *.o *.so* *.lo .libs
