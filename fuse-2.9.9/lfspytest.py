import os
import sys

path="testdir/hello"
fd=open(path,"w")
fd.write("hello")
fd.close()
fd=open(path,"r")
contents=fd.read()
fd.close()
print(contents)
