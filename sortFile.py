#!/usr/bin/env python

import sys

f = open(sys.argv[1], 'r')
lines = f.readlines()
f.close()
f = open(sys.argv[1], 'w')
lines.sort()
f.writelines(lines)
f.close()
