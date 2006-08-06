
include ../../directives.mk

include ../../directives.runtime.mk

include ../../flags.mk
			
###########################################################

override CPPFLAGS:=-I../../include $(CPPFLAGS)
override LDFLAGS:=-L../../ $(LDFLAGS) 

test: test.cc
	$(CXX) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) $(CFLAGS) $(DEBUG) -ldodo $^ -o $@ 

compare: compare.sql.cc
	$(CXX) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) $(CFLAGS) $(DEBUG) -ldodo $^ -o $@ 
	
clean:
	rm -rf test compare .libs
