import os
import sys
import SCons

from os.path import basename, dirname, exists, isdir, isfile, join as joinpath

env = Environment(ENV = os.environ)

env['BUILDROOT'] = 'build'

src_list = []

def AddSource(file):
	src_list.append(File(src_list))
Export('AddSource')


SConscript('SConscript', variant = 'build')

env.Program('test', src_list)