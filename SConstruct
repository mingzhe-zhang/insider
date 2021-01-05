import os
import sys
import SCons

from os.path import basename, dirname, exists, isdir, isfile, join as joinpath

env = Environment(ENV = os.environ)

env['BUILDROOT'] = 'build'


src_list = []
inc_list = []
inc_list.append('inc')

def AddSource(file):
	src_list.append(File(file))
Export('AddSource')


SConscript('./SConscript', variant_dir = 'build')

env.Clean('build/test', 'build')

env.Program('build/bin/test', src_list, CPPPATH=inc_list)