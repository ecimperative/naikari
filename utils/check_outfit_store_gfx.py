#!/usr/bin/python

from glob import glob
import os.path
import re

directory = os.path.split(os.getcwd())
if directory[1] == 'utils':
    prefix = '..'
elif directory[1] == 'naev':
    prefix = '.'
else:
    print("Failed to detect where you're running this script from\nPlease enter your path manually")

images  = glob(prefix+"/dat/gfx/outfit/store/*")
images += glob(prefix+"/artwork/gfx/outfit/store/*")
images  = list(map( lambda x: os.path.basename(x), images ))

imgdict = {}
for i in images:
    imgdict[i] = 0

for file in glob(prefix+"/dat/outfits/**/*.xml"):
    with open( file, 'r' ) as f:
        m = re.search( "<gfx_store>(.+?)</gfx_store>", f.read() )
        if m:
            s = m.group(1)
            v = imgdict.get(s)
            if not v:
                v = 0
            imgdict[s] = v+1

overused = []
underused = []
for k,v in imgdict.items():
    if "organic" in k:
        continue
    if v > 1:
        overused += [k]
    elif v==0:
        underused += [k]

with open( "outfit_gfx.html", "w" ) as out:
    out.write( """
<html>
<head>
 <title>Naev Outfit Graphic Used Status</title>
</head>
<body>
    """ )
    for k in sorted( imgdict, key=lambda x: imgdict[x], reverse=True ):
        path = prefix+"/dat/gfx/outfit/store/"+k
        if not os.path.isfile( path ):
           path = prefix+"/artwork/gfx/outfit/store/"+k
        out.write(f"<div><img src='{path}' /> {k}: {imgdict[k]}</div>\n")
    out.write( """
</body>
</html>
    """ )

print( "OVERUSED ASSETS:")
s = sorted(overused, key=lambda x: imgdict[x], reverse=True)
for k in s:
    print( f"   {k}: {imgdict[k]}" )

print( "UNDERUSED ASSETS:")
for k in underused:
    print( f"   {k}" )
