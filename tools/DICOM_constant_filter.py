# DICOM_constant_filter.py
# Strip out a list of all constant names from the list of source files in the location provided
# and output C source of an array of {char*,int}
from __future__ import print_function
import re
import os

InstallDir = os.path.join("C:\\", "lib") 
DICOMBase = os.path.join(InstallDir, "DCMTK-debug", "include", "dcmtk", "dcmdata")
files = [
	"dcdeftag.h"
]

for f in files:
	filename = os.path.join(DICOMBase, f)
	print(filename)
	fh = open(filename)
	for line in fh:
		for tok in line.split(): # split on whitespace
			if re.match("^DCM_", tok): # match SDL constants
				print('{{ "{0}", {0} }},'.format(tok))
	fh.close()
