#!/usr/bin/env python

from os import system
from glob import glob

g = glob("*.cpp") + glob("*.h");
for f in g:
	command = "sed -e s/wxkanji/jben/ < " + f + "> " + f + "2"
	print command
	system(command)
	command = "sed -e s/wxKanji/JBen/ < " + f + "2 > " + f + "3"
	print command
	system(command)
