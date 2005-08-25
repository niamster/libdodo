LIBTOOL = libtool
CXX = g++
CFLAGS=-O3 -fmove-all-movables -march=pentium4
OBJECTS=base.o dodoBase.o tools.o xexec.o sqlBaseEx.o sqlBase.o baseEx.o mysqlinterface.o mysqlinterfaceEx.o

override DEFINES:=$(DEFINES)

override CPPFLAGS:=-I/opt/mysql/include/mysql/ $(CPPFLAGS)
override LDFLAGS:=-L/opt/mysql/lib/mysql -L./ $(LDFLAGS)

override LIBS:=-lmysqlclient $(LIBS)

DODO_INCLUDE=-I./ 
DODO_LIBRARY=-L./

LIBRARY=dodo

VERSION = 0

MINOR = 1

$(LIBRARY): $(OBJECTS)
	$(LIBTOOL) --mode=link $(CXX) $(LDFLAGS) $(LIBS) -shared -Wl,-soname,lib$@.so.$(VERSION).$(MINOR) -o lib$@.so.$(VERSION).$(MINOR) $^
	ln -fs lib$@.so.$(VERSION).$(MINOR) lib$@.so
	ldconfig -n ./
.cc.o:
	$(CXX) $(CPPFLAGS) $(CFLAGS) -fpic -c $^
	strip -d $@
	
clean:
	rm -rf *.o *.so* *.lo .libs

force : clean $(LIBRARY)
