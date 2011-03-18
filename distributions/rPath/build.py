#!/usr/bin/python

trove = 'libdodo-hg.recipe'

def useCombinations(uses, extra=[]):
    builds = []

    n = len(uses)
    B = [0 for i in range(0, n)]
    i = 0
    p = 0 
    while p < n:
        flavors = []
        for o in range(0, n):
            if B[o]:
                flavors += [uses[o]]
            else:
                flavors += ["!" + uses[o]]
        builds += [ "'%(trove)s[" + ",".join(flavors + extra) + "]'" ]
        i = i + 1
        p = 0
        j = i
        while j % 2 == 0:
            j = j/2
            p = p + 1
        if p <= n:
            B[p-1] = 1 - B[p-1]

    return builds

def useOneOtherNone(uses):
    builds = []
    
    for i in uses:
        builds += [ "'%(trove)s[" + ",".join(["!" + i] + [x for x in uses if x != i]) + "]'" ]

    return builds

def useNotOneOtherAll(uses):
    builds = []
    
    for i in uses:
        builds += [ "'%(trove)s[" + ",".join([i] + ["!" + x for x in uses if x != i]) + "]'" ]

    return builds

rmake = ["rmake", "build"]

uses = ("bzip2", "dl", "fastcgi", "iconv", "imagemagick", "libxml2", "mysql", "pcre", "postgresql", "sqlite3", "zlib", "ssl", "ipcxsi")

xexecFlags = ("databaseXexec", "graphicsXexec", "ioXexec")

#critical cases
rmake += [ "'%(trove)s[" + ",".join([x for x in uses]) + "]'" ]
rmake += [ "'%(trove)s[" + ",".join([x for x in uses] + ["!" + x for x in xexecFlags]) + "]'" ]

for i in ("libxml2", "deque", "dl", "ssl"):
    rmake += [ "'%(trove)s[" + ",".join(["!" + i] + [x for x in uses if x != i]) + "]'" ]

#general
#rmake += useOneOtherNone(uses)
#rmake += useNotOneOtherAll(uses)

uses = ()
rmake += useCombinations(uses)

print ' '.join(rmake) % {'trove': trove}
