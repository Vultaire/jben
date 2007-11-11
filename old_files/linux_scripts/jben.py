#!/usr/bin/env python

from os import system
from glob import glob

g = glob("*.cpp") + glob("*.h");
for f in g:
	command = "mv " + f + " " + f + "~"
	print command
	system(command)
	command = "mv " + f + "3 " + f
	print command
	system(command)
