#!/usr/bin/env python
#

from mercurial import demandimport; demandimport.enable()

from mercurial.hgweb.hgweb_mod import hgweb
import mercurial.hgweb.wsgicgi as wsgicgi

application = hgweb('/var/hg/libdodo/')
wsgicgi.launch(application)
