#!/usr/bin/env python
#

import cgitb
cgitb.enable()

from mercurial import demandimport; demandimport.enable()

from mercurial.hgweb.hgweb_mod import hgweb
from mercurial.hgweb.request import wsgiapplication
from flup.server.fcgi import WSGIServer

def application():
	return hgweb("/var/hg/libdodo/", "libdodo")

WSGIServer(wsgiapplication(application)).run()
