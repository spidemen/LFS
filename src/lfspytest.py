import os
import sys

path="temp/foo"
fd=open(path,"w")
fd.write("hello")
fd.close()
fd=open(path,"r")
contents=fd.read()
fd.close()
print(contents)
