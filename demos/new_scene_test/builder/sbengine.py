/* 
 *  sbengine.py
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2009  Saarland University
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Created on: 2009-02-24 16:01:56 +0100
 *  Author(s): Felix Klein
 */
 
 #-*- Python -*-
# SCons Builder Engine V2.2

# NOTE: This file is executed by the bootstrap loader in the context of
#       the project root directory. 

# NOTE: Starting with the version 2.0 SB Engine require SCons v0.98
#       or more precisely SCons v0.97.0d20070809 (with AddMethod) and later.
#       Because there is no easy way to check such checkpoint version
#       and because Ubuntu is shipped with SCons v0.97.0d20071203
#       we avoid to check for 0.98 directly.
EnsureSConsVersion(0, 97, 0)
try:
    AddMethod
except NameError:
    print 'Error: You need at least SCons v0.97.0d20070809 in order to run'\
        ' this SConstruct file'
    raise

import os
import os.path
import sys

from builder.btools import *
import builder.bconfig

# Check architecture

arch = GetArchitectureId()

if not arch:
    arch = 'x86'

print '* Current architecture:', arch

if IsArchitectureSupported(arch):
    SetDefaultArchitectureName(arch)
else:
    print '* Unsupported architecture:%s, x86 will be used instead' % arch
    SetDefaultArchitectureName('x86')

# Print platform information

defaultPlatform = Platform()
allowedPlatforms = ('posix', 'cygwin', 'win32', 'irix', 'sunos',
                    'hpux', 'aix', 'darwin', 'os2')

print '* Current platform:',defaultPlatform

# Determine platform to use for build

platform = ARGUMENTS.get('PLATFORM', defaultPlatform)

# Determine build directory

buildDir = ARGUMENTS.get('build', 'build')

while os.path.islink(buildDir):
    # resolve symbolic links
    buildDir = os.path.join(os.path.dirname(buildDir), os.readlink(buildDir))

# Create build directory, inclusive all intermediate-level directories if
# it does not exists
if not os.path.isdir(buildDir):
    if os.path.exists(buildDir):
        print '%s is not a directory' % buildDir
        Exit(1)
    else:
        os.makedirs(buildDir)

# Setup initial environment

if os.path.isabs(buildDir):
    BUILD_DIR = buildDir
else:
    BUILD_DIR = '#' + buildDir

OBJ_DIR = BUILD_DIR + os.sep + 'obj'

env = Environment(platform=platform,
                  tools=['gnulink', 'ar', 'subst'],
                  toolpath=['builder/scons_tools'],
                  BUILD_DIR=BUILD_DIR,
                  OBJ_DIR=OBJ_DIR)

# Update external environment
for varName in ('PATH', 'LD_LIBRARY_PATH'):
    if os.environ.has_key(varName):
        env.AppendENVPath(varName, os.environ[varName])

# put all .sconsign files into a big one
env.SConsignFile(Dir(BUILD_DIR).abspath + os.sep + '.sconsignatures')

print '* Selected build platform:', env['PLATFORM']
print '* Selected build directory:', buildDir

# Load project file

loaded = False
for f in ('SConsProject', 'Sconsproject', 'sconsproject'):
    if os.path.exists(f):
        print '* Loading project file:', f
        SConscript(f, exports={'init_env' : env})
        loaded = True
        break
if not loaded:
    print >>sys.stderr, 'Error: Could not found SConsProject file'
    Exit(1)

# Load configuration options

# In <buildDir>/build.conf are cached options of the last run
optionsCacheFile = os.path.join(buildDir, 'build.conf')

# Get list of additional configuration files
configFiles = ARGUMENTS.get('config', None)
if configFiles:
    configFiles = configFiles.split(os.pathsep)
else:
    configFiles = []

OPTIONS_FILES = [optionsCacheFile] + configFiles

ALL_OPTIONS = [
    ('CC', 'The C compiler.'),
    ('CXX', 'The C++ compiler.'),
    EnumOption('compiler', 'select compiler configuration',
               GetDefaultCompilerName(),
               allowed_values=GetCompilerNames()),
    EnumOption('arch', 'select architecture configuration',
               GetDefaultArchitectureName(),
               allowed_values=GetArchitectureNames()),
    EnumOption('PLATFORM', 'select target platform',
               str(defaultPlatform),
               allowed_values=allowedPlatforms),
    EnumOption('optimization', 'optimization mode', 'medium',
               allowed_values=('debug', 'none', 'medium', 'high', 'extreme')),
    BoolOption('warnings', 'compilation with -Wall and similar', 1),
    EnumOption('debug', 'debug symbols and macros (macro option disables NDEBUG)', 'no',
               allowed_values=('no', 'symbols', 'macro', 'full'),
               map={}, ignorecase=0), # case sensitive
    ('prefix', 'Installation prefix directory', '/usr/local'),
    ('libpath',
     'Additional library paths to use (separated by \''+os.pathsep+'\')'),
    ('cpppath',
     'Additional include paths to use (separated by \''+os.pathsep+'\')'),
    ('ccflags', 'Additional C/C++ flags to use (separated by spaces)'),
    ('linkflags', 'Additional linker flags to use (separated by spaces)'),
    BoolOption('profile', 'compile and link with -pg option (gprof)', 0),
    BoolOption('staticlibs', 'create static instead of dynamic libraries', 0),
    PackageOption('gnuwin32', 'path to gnuwin32 package', 'no'),
    PackageOption('mingw', 'path to MinGW package', 'no'),
    ]

# Append all user defined options (see builder/bconfig.py)

ALL_OPTIONS.extend(GetCustomOptions())

def GetOptionNames(optlist, exclude_options=[]):
    return [o[0] for o in optlist if o[0] not in exclude_options]

# Load cached options

print '* Loading configuration files:',' '.join(OPTIONS_FILES)

OPTIONS_TO_CACHE = GetOptionNames(ALL_OPTIONS)

# In CACHED_OPTIONS are all options saved last time in the cache
CACHED_OPTIONS = LoadOptions([optionsCacheFile], OPTIONS_TO_CACHE)
env.Replace(CACHED_OPTIONS=CACHED_OPTIONS)

# In CONFIG_FILE_OPTIONS are all options loaded from external config file
CONFIG_FILE_OPTIONS = LoadOptions(configFiles, OPTIONS_TO_CACHE)
env.Replace(CONFIG_FILE_OPTIONS=CONFIG_FILE_OPTIONS)

# In SPECIFIED_OPTIONS are put all options specified over different sources:
# cache, command-line arguments, and finally user config file.
# Order of the updating is important: cached options are overwritten by
# options from config file, which again are overwritten by command-line
# arguments.

SPECIFIED_OPTIONS = {}
SPECIFIED_OPTIONS.update(CACHED_OPTIONS)
SPECIFIED_OPTIONS.update(CONFIG_FILE_OPTIONS)
SPECIFIED_OPTIONS.update(ARGUMENTS)
env.Replace(SPECIFIED_OPTIONS=SPECIFIED_OPTIONS)

# Print cached options if showconf is specified
if 'showconf' in COMMAND_LINE_TARGETS:
    def printOptions(optdict):
        for k, v in optdict.items():
            print '%s = %s' % (k,repr(v))
        
    print '==== Cached options ===='
    printOptions(CACHED_OPTIONS)
    print '==== Config file options ===='
    printOptions(CONFIG_FILE_OPTIONS)
    print '==== Specified options ===='
    printOptions(SPECIFIED_OPTIONS)
    Exit(0)

# Setup options
opts = Options(None, SPECIFIED_OPTIONS)
apply(opts.AddOptions, ALL_OPTIONS)
opts.Update(env)

# Generate help
Help("""
---- Command-line variable=value options ----

config: Additional configuration files (separated by '%(pathsep)s').
        Options from the configuration files will be stored in the cache.

build: Build directory.
""" % {'pathsep' : os.pathsep})

Help(opts.GenerateHelpText(env))

Help("""
Use cpppath+="PATH" and libpath+="PATH" for adding additional paths to the cache.
Use cpppath-="PATH" and libpath-="PATH" for removing paths from the cache.

Similarily for ccflags and linkflags.

---- Command-line targets ----

showconf:       Print cached options
""")

print '* Selected optimization mode:', env['optimization']

# Misc environment setup

if env['staticlibs']:
    print '* Building and linking statically'
else:
    print '* Building and linking dynamically'

if env['gnuwin32']:
    env.Append(CPPPATH=env['gnuwin32']+os.sep+'include')
    env.Append(LIBPATH=env['gnuwin32']+os.sep+'lib')
    env.AppendENVPath('PATH', env['gnuwin32']+os.sep+'bin')

if env['mingw']:
    env.AppendENVPath('PATH', env['mingw']+os.sep+'bin')

# check for 'XXX+=YYY' arguments for every pathvar,
# and append path YYY to environment variable XXX
# the same for 'XXX-=YYY' but remove YYY from XXX instead

for pathVar in ('libpath', 'cpppath'):
    pathToAppend = ARGUMENTS.get(pathVar+'+')
    pathToRemove = ARGUMENTS.get(pathVar+'-')

    varValue = env.get(pathVar, '')
    
    if pathToAppend:
        varValue = AppendPath(varValue, pathToAppend)
    if pathToRemove:
        varValue = RemovePath(varValue, pathToRemove)

    env[pathVar] = varValue


for flagsVar in ('ccflags', 'linkflags'):
    flagsToAppend = ARGUMENTS.get(flagsVar+'+')
    flagsToRemove = ARGUMENTS.get(flagsVar+'-')

    varValue = env.get(flagsVar, '')
    
    if flagsToAppend:
        varValue = ' '.join(varValue.split() + flagsToAppend.split())
    if flagsToRemove:
        flagsToRemove = flagsToRemove.split()
        flags = varValue.split()
        newFlags = [f for f in flags if f not in flagsToRemove]
        varValue = ' '.join(newFlags)

    env[flagsVar] = varValue

try:
    env = env.SetupEnvironment()
finally:
    # Save current configuration
    opts.Save(optionsCacheFile, env)

# Activate logging

#logfile = open('log.txt', 'w')

#def log_cmd_line(s, target, source, env):
#  sys.stdout.write(s + "\n")
#  logfile.write(s+"\n")

#env['PRINT_CMD_LINE_FUNC'] = log_cmd_line

# Dump configuration

if ARGUMENTS.has_key('dump'):
    envName = ARGUMENTS['dump']
    dumpKey = ARGUMENTS.get('dump_key')

    dumpEnv = globals()[envName]
    
    if not dumpKey:
        prefix = envName + '.Dictionary()'
        dumpKey = None
    else:
        prefix = envName + '.Dictionary( ' + dumpKey + ' )'
        
    dumpEnv.DumpEnv(key = dumpKey,
                    header = prefix + ' - start',
                    footer = prefix + ' - end')
