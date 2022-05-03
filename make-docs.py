#!/usr/bin/env python3
import os
import shutil

if shutil.which('doxygen') is None:
    exit("You must install doxygen")

# clean
if os.path.exists('docs'):
    shutil.rmtree('docs')

# build
if os.system('doxygen docsrc/doxygen.config') != 0:
    exit(1)
