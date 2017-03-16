/* 
 *  bconfig.py
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
 
 # SCons Builder Engine Configuration V2.1

# Configuration
from btools import *
import sys
import os
import os.path
import getpass
import re
from string import split
import SCons.Util

# Utilities

def filterOut(patterns, textList):
    if isinstance(patterns, str):
        patterns = patterns.split()
    if isinstance(textList, str):
        textList = textList.split()
        rejoin = True
    else:
        rejoin = False

    res = filter(lambda i: i not in patterns, textList)
    if rejoin:
        res = ' '.join(res)
    return res

def loadModules(modulesPath):
    modules = []
    if os.path.exists(modulesPath):
        # list all "modules/*.py" files
        modules = [ p[:-3] for p in os.listdir(modulesPath) if p.endswith('.py') ]
    oldSysPath = sys.path
    sys.path = [modulesPath] + sys.path
    print '* Loading configuration modules:',
    for m in modules:
        mod = __import__(m)
        print m,
    sys.path = oldSysPath
    print '.'

def getProjectPrefix(env):
    return env.get('PROJECT_PREFIX', '_')

def getProjectSystemPrefix(env):
    return getProjectPrefix(env)+env.get('SYSTEM_PREFIX','')
    
def getProjectCompilerPrefix(env):
    return getProjectPrefix(env)+env.get('COMPILER_PREFIX','')

def getAutoconfPrefix(env):
    return env.get('AUTOCONF_PREFIX','')

# Architecture Info

# Architecture ID
ARCH_UNKNOWN  = 0
ARCH_X86      = 1
ARCH_X86_64   = 2
ARCH_IA64     = 3

# CPU ID
CPU_UNKNOWN   = 0
CPU_X86       = 1
CPU_PENTIUM_3 = 2
CPU_PENTIUM_4 = 3
CPU_PENTIUM_M = 4
CPU_CORE      = 5
CPU_CORE_2    = 6
CPU_OPTERON   = 7
CPU_ITANIUM   = 8

class ArchInfo:

    def __init__(self, env):
        arch = env['arch']
        if arch == 'PentiumIII':
            self.arch = ARCH_X86
            self.cpu = CPU_PENTIUM_3
        elif arch in ('x86', 'i386', 'i586', 'i686', 'PentiumIV'):
            self.arch = ARCH_X86
            self.cpu = CPU_PENTIUM_4
        elif arch == 'PentiumM':
            self.arch = ARCH_X86
            self.cpu = CPU_PENTIUM_M
        elif arch == 'Core2':
            self.arch = ARCH_X86
            self.cpu = CPU_CORE_2
        elif arch in ('x86_64', 'Opteron'):
            self.arch = ARCH_X86_64
            self.cpu = CPU_OPTERON
        elif arch == 'Itanium':
            self.arch = ARCH_IA64
            self.cpu = CPU_ITANIUM

def getArchInfo(env):
    archInfo = env.get('SB_ARCH_INFO', None)
    if not archInfo:
        archInfo = ArchInfo(env)
        env.Replace(SB_ARCH_INFO=archInfo)
    return archInfo

# Custom Options

AddCustomOptions(
    ('IA32ROOT', "Path to the Intel's icc compiler. Specific to Intel compiler."),
    EnumOption('intel_link_libintel', 'Mode for linking of the Intel provided libraries. Specific to Intel compiler.', 'shared', ['shared', 'static']),
    EnumOption('intel_link_libgcc', 'Mode for linking of the libgcc C++ library. Specific to Intel compiler.', 'shared', ['shared', 'static']),
    ('MSVS_VERSION', 'Version of MSVS to use'),
    PackageOption('msvc_compiler_dir',
                  'path to MSVC compiler directory', 'no')
    )

# Custom Configuration

def CheckCCompiler(ctx):
    ctx.Message('Checking C compiler... ')
    ret, outputStr = ctx.TryRun("""
    #include <stdio.h>
    int main(int argc, char **argv) {
      printf("Hello world");
      return 0;
    }
    """, extension='.c')
    
    if ret and outputStr:
        if outputStr.strip() != 'Hello world':
            ctx.Message('Compilation failed')
            ctx.Result(0)
            return 0
    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckCCompiler', CheckCCompiler)

def CheckTypeSizes(ctx, write_config_h=True):
    ctx.Message('Checking type sizes... ')
    ret, outputStr = ctx.TryRun("""
    #include <stdio.h>
    int main(int argc, char **argv) {
      printf("%i %i %i %i\\n", sizeof(char), sizeof(short), sizeof(int), sizeof(long));
      return 0;
    }
    """, extension='.c')
    
    ret1, outputStr1 = ctx.TryRun("""
    #include <stdio.h>
    int main(int argc, char **argv) {
      printf("%i\\n", sizeof(long long));
      return 0;
    }
    """, extension='.c')

    symprefix = getAutoconfPrefix(ctx.env)
    
    if ret and outputStr:
        sizes = map(int, outputStr.split())
        if not (sizes[0] == 1 and \
                sizes[0] <= sizes[1] and \
                sizes[1] <= sizes[2] and \
                sizes[2] <= sizes[3]):
            ctx.Message('Wrong type sizes : char : %i, short : %i, int : %i, long : %i. ' % tuple(sizes))
            ctx.Result(0)
            return 0

        if ret1:
            sizes += map(int, outputStr1.split())

        names = ['char', 'short', 'int', 'long', 'long long']
        defs = []
        
        # define sizeof int
        k, v = symprefix+'SIZE_OF_INT', sizes[2]

        if not (write_config_h and AddConfigKey(ctx, k, v)):
            defs.append((k, v))
        
        
        # int8 is always char !
        # int16
        try:
            k, v = symprefix+'INT16_TYPE', names[sizes.index(2)]
            if not (write_config_h and AddConfigKey(ctx, k, v)):
                defs.append((k, v))
        except ValueError:
            ctx.Message('No 16-bit integer ')
            ctx.Result(0)
            return 0

        # int32
        try:
            k, v = symprefix+'INT32_TYPE', names[sizes.index(4)]
            if not (write_config_h and AddConfigKey(ctx, k, v)):
                defs.append((k, v))
            
        except ValueError:
            ctx.Message('No 32-bit integer ')
            ctx.Result(0)
            return 0

        # int64
        try:
            k, v = symprefix+'INT64_TYPE', names[sizes.index(8)]
            if not (write_config_h and AddConfigKey(ctx, k, v)):
                defs.append((k, v))
        except ValueError:
            ctx.Message('No 64-bit integer ')
        
        ctx.env.Append(CPPDEFINES=defs)
    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckTypeSizes', CheckTypeSizes)

def CheckEndianness(ctx, write_config_h=True):
    ctx.Message('Checking platform endianness... ')
    ret, outputStr = ctx.TryRun("""
    #include <stdio.h>
    int main(int argc, char **argv) {
      // From Harbison&Steele, adopted from cmake
      union
      {
        long l;
        char c[sizeof (long)];
      } u;
      u.l = 1;
      if (u.c[sizeof (long) - 1] == 1)
        printf("big\\n");
      else
        printf("little\\n");
      return 0;
    }
    """, extension='.c')
    
    symprefix = getAutoconfPrefix(ctx.env)
    
    if ret:
        be_key = symprefix+'BIG_ENDIAN'
        le_key = symprefix+'LITTLE_ENDIAN'
        
        if outputStr.find('big') != -1:
            if write_config_h and AddConfigKey(ctx, be_key, 1):
                AddConfigKey(ctx, le_key, 0)
            else:
                ctx.env.Append(CPPDEFINES=[be_key])
        elif outputStr.find('little') != -1:
            if write_config_h and AddConfigKey(ctx, le_key, 1):
                AddConfigKey(ctx, be_key, 0)
            else:
                ctx.env.Append(CPPDEFINES=[le_key])
        else:
            if write_config_h:
                AddConfigKey(ctx, le_key, 0)
                AddConfigKey(ctx, be_key, 0)
            ctx.Message('Could not determine platform endianess')
            ctx.Result(0)
            return 0
    
    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckEndianness', CheckEndianness)

def CheckPthreads(ctx, write_config_h=True):
    ctx.Message('Checking for pthreads... ')
    platform = ctx.env['PLATFORM']
    lastLIBS = list(ctx.env.get('LIBS', []))

    for pthreadlib in ('pthread',):
        ctx.env.Append(LIBS = [pthreadlib])
        ret = ctx.TryLink("""
    #include <pthread.h>
    void *thread_main(void *dummy) { }
       
    int main(int argc, char **argv) {
      pthread_t handle;
      pthread_create(&handle, 0, &thread_main, NULL);
      return 0;
    }
    """, extension='.c')
        ctx.env.Replace(LIBS = lastLIBS)
        if ret:
            ctx.env.Replace(LIBPTHREAD=[pthreadlib])
            break
    key = getAutoconfPrefix(ctx.env)+'HAVE_LIBPTHREAD'
    if not (write_config_h and AddConfigKey(ctx, key, ret)):
        # no config file is specified or it is disabled, use compiler options
        if ret:
            ctx.env.Append(CPPDEFINES=[key])

    ctx.Result(ret)
    return ret

RegisterCustomTest('CheckPthreads', CheckPthreads)

# Custom Environment

def customizeEnvironment(env):
    # setup all basic libraries

    platform = env['PLATFORM']
    
    env.Replace(LIBPTHREADS=['pthread'])
    if platform == 'win32':
        env.Replace(LIBDL=[])
        # compatibility dl package
        env.DeclarePackage('dl', vars={}, trigger_libs=['dl'])
    else:
        env.Replace(LIBDL=['dl'])
        env.DeclarePackage('dl', trigger_libs=['dl'], LIBS=['dl'])
        
    # TODO: this setup is only correct for Linux
    env.Replace(LIBX11DIR=['/usr/X11R6/lib', '/usr/X11R6/lib64'])
    env.Replace(LIBXMU=['Xmu'])
    env.Replace(LIBXRENDER=['Xrender'])
    env.Replace(LIBGL=['GL']) 
    env.Replace(LIBX11=['Xext', 'X11'])
    env.Replace(LIBMATH=['m'])
    env.Replace(INCLUDEX11='/usr/X11R6/include')

    #env.Append(LIBS=['$LIBDL', '$LIBMATH'])
    #env.Append(CPPDEFINES=['__%s__' % getpass.getuser()])

RegisterEnvironmentCustomizer(customizeEnvironment)

# Compilers

# ARGUMENTS variable can be used to find out which environment variables
# were set by the command line arguments

def parseCompilerVersion(s):
    m = re.search(r'([0-9]+)\.([0-9]+)(?:\.([0-9]+))?', s)
    if m:
        major, minor, patch = map(lambda x: x and int(x), m.groups())
        if patch is None:
            patch = 0
        versionid = major*10000 + minor*100 + patch
        return (major, minor, patch, versionid)
    else:
        return (0, 0, 0, 0)

def parseIntelCompilerVersion(s):
    """this function can parse version information printed
    by -v and -V options.
    Returns tuple (minor, major, patch, buildid, versionid)
    where minor, major, patch and versionid are ints and buildid is string
    """
    m = re.search(r'Version\s+([0-9]+)\.([0-9]+)(?:\s+Build\s+([^\s]+)\s+Package\sID:\s[^\s0-9]+([0-9]+)\.([0-9]+)\.([0-9]+))?', s)
    if m:
        major = int(m.group(1))
        minor = int(m.group(2))
        buildid = m.group(3)
        if m.group(4) and int(m.group(4)) == major and \
               m.group(5) and int(m.group(5)) == minor:
            patch = int(m.group(6))
        else:
            patch = 0
        versionid = major*10000 + minor*100 + patch
        return (major, minor, patch, buildid, versionid)
    else:
        return (0, 0, 0, '', 0)

# misc MSVC utilities

def setupMsvcTool(env, msvcCompilerDir, toolName='msvc'):
    """returns modified msvcCompilerDir"""
    if msvcCompilerDir:
        compilerFound = False
        for d in [msvcCompilerDir,
                  os.path.join(msvcCompilerDir, 'bin'),
                  os.path.join(msvcCompilerDir, 'VC', 'bin')]:
            if env.WhereIs('cl', d):
                env.PrependENVPath('PATH', d)
                msvcCompilerDir = d
                compilerFound = True
                break
        if not compilerFound:
            print >>sys.stderr, 'Error: No compiler found in %s directory' % \
                  msvcCompilerDir
            msvcCompilerDir = None
            env.Exit(1)
        else:
            print '** Use MSVC compiler from directory %s' % msvcCompilerDir

    env.Tool(toolName)

    if msvcCompilerDir:
        # loading msvc tool may change env['ENV']['PATH'], thus repeat update
        env.PrependENVPath('PATH', msvcCompilerDir)
        
        # check for include and lib directories
        includeDir = os.path.normpath(os.path.join(msvcCompilerDir,
                                                   '..', 'include'))
        if os.path.exists(includeDir):
            env.PrependENVPath('INCLUDE', includeDir)
            
        libDir = os.path.normpath(os.path.join(msvcCompilerDir,
                                               '..', 'lib'))
        if os.path.exists(libDir):
            env.PrependENVPath('LIB', libDir)

    # Check MSVS version number
    
    MSVS = env.get('MSVS')
    if not MSVS:
        print >>sys.stderr, 'Error: Could not find MSVC/MSVS C/C++ compiler'
        env.Exit(1)

    MSVS_VERSION = MSVS['VERSION']

    (vmaj, vmin, vpatch, versionid) = parseCompilerVersion(MSVS_VERSION)
    
    print '** Determined MSVC/MSVS Compiler Version %s' % MSVS_VERSION
    print '** All Available: %s' % (' '.join(MSVS['VERSIONS']))

    # Workaround for SCons versions which does not know about MSVS 9.0
    if versionid >= 90000 and MSVS.has_key('VCINSTALLDIR') and \
            (not env['ENV'].has_key('INCLUDE') or \
                 MSVS['VCINSTALLDIR'] not in env['ENV']['INCLUDE']):

        print '** Applying workaround for MSVS 9.x'
        # -- INCLUDE --
        env.PrependENVPath('INCLUDE', os.path.join(MSVS['VCINSTALLDIR'], 
                                                   'INCLUDE'))
        env.PrependENVPath('INCLUDE', os.path.join(MSVS['VCINSTALLDIR'], 
                                                   'ATLMFC', 'INCLUDE'))

        # -- LIB --
        env.PrependENVPath('LIB', os.path.join(MSVS['VCINSTALLDIR'], 'LIB'))
        env.PrependENVPath('LIB', os.path.join(MSVS['VCINSTALLDIR'],
                                               'ATLMFC', 'LIB'))
        # -- LIBPATH --
        env.PrependENVPath('LIBPATH', os.path.join(MSVS['VCINSTALLDIR'], 'LIB'))
        env.PrependENVPath('LIBPATH', os.path.join(MSVS['VCINSTALLDIR'], 
                                                   'ATLMFC', 'LIB'))
        env.PrependENVPath('LIBPATH', os.path.join(MSVS['FRAMEWORKDIR'], 
                                                   MSVS['FRAMEWORKVERSION']))

        # -- PATH --
        env.PrependENVPath('PATH', os.path.join(MSVS['VCINSTALLDIR'],
                                                'VCPackages'))
        env.PrependENVPath('PATH', os.path.join(MSVS['FRAMEWORKDIR'], 
                                                MSVS['FRAMEWORKVERSION']))
        env.PrependENVPath('PATH', os.path.join(MSVS['VSINSTALLDIR'],
                                                'Common7', 'Tools'))
        env.PrependENVPath('PATH', os.path.join(MSVS['VCINSTALLDIR'], 'BIN'))
        env.PrependENVPath('PATH', os.path.join(MSVS['VSINSTALLDIR'], 
                                                'Common7', 'IDE'))

    if versionid >= 80000: # MSVS 8.0
        # add support for manifests
        print '** Using mt.exe for adding manifests to the binaries'
        env.Replace(SHLINKCOM=[ env['SHLINKCOM'], 'mt.exe -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;2' ])
    
    # Check for Platform SDK
    
    PLATFORMSDKDIR = MSVS.get('PLATFORMSDKDIR')
    if not PLATFORMSDKDIR and SCons.Util.can_read_reg:
        # Try alternative check for PSDK (portions from msvs.py SCons tool)
        winMajorVer = sys.getwindowsversion()[0]
        
        # list of tuples (main_registry_key, installation_path_subkey)
        regKeysToCheck = [
            (r'Software\Microsoft\Microsoft SDKs\Windows', 
             'installationfolder'), # MS SDK v6.0a (later versions too?)
            (r'Software\Microsoft\MicrosoftSDK\InstalledSDKs',
             'install dir')
            ]

        ## Windows Vista
        #if winMajorVer >= 6:
        #    location = r'Software\Microsoft\Microsoft SDKs\Windows'
        #    keyValue = 'installationfolder'

        ## Windows XP and older
        #else:
        #    location = r'Software\Microsoft\MicrosoftSDK\InstalledSDKs'
        #    keyValue = 'install dir'
        
        rootKeys = [SCons.Util.HKEY_LOCAL_MACHINE, SCons.Util.HKEY_CURRENT_USER]
        
        installDirs = []

        for location, keyValue in regKeysToCheck:
            try:
                for rootKey in rootKeys:
                    k = SCons.Util.RegOpenKeyEx(rootKey, location)
                    i = 0
                    while 1:
                        try:
                            key = SCons.Util.RegEnumKey(k,i)
                            sdk = SCons.Util.RegOpenKeyEx(k,key)
                            j = 0
                            installDir = ''
                            while 1:
                                try:
                                    (vk,vv,t) = SCons.Util.RegEnumValue(sdk,j)

                                    if vk.lower() == keyValue:
                                        installDir = vv
                                        break
                                    j = j + 1
                                except SCons.Util.RegError:
                                    break
                            if installDir:
                                installDirs.append(installDir)
                            i = i + 1
                        except SCons.Util.RegError:
                            break
            except SCons.Util.RegError:
                pass

        for dir in installDirs:
            if os.path.isdir(os.path.join(dir, 'Include')) and \
                   os.path.isdir(os.path.join(dir, 'Lib')):
                PLATFORMSDKDIR = dir
                break
    
    if PLATFORMSDKDIR:
        print '** Determined Platform SDK Path: %s' % PLATFORMSDKDIR
        env.AppendENVPath('INCLUDE', os.path.join(PLATFORMSDKDIR, 'Include'))
        env.AppendENVPath('LIB', os.path.join(PLATFORMSDKDIR, 'Lib'))
        env.AppendENVPath('PATH', os.path.join(PLATFORMSDKDIR, 'bin'))
        # Following does not work correctly :
        #env.Append(CPPPATH=[os.path.join(PLATFORMSDKDIR, 'Include')])
        #env.Append(LIBPATH=[os.path.join(PLATFORMSDKDIR, 'Lib')])
    else:
        print '** Warning: Platform SDK not found, please specify the paths manually'

    class _setup_result:
        pass

    r = _setup_result()
    r.msvcVersionMajor = vmaj
    r.msvcVersionMinor = vmin
    r.msvcVersionPatch = vpatch
    r.msvcVersionId = versionid
    r.msvcCompilerDir = msvcCompilerDir
    r.PLATFORMSDKDIR = PLATFORMSDKDIR
    return r

def getIntelCompilerVersion(env):
    # check version number (on Windows icl prints version by default)
    isWin32 = (env['PLATFORM'] == 'win32')
    cmd = env['CXX']

    if not isWin32:
        # on posix you need -V to get full version
        cmd += ' -V'
    
    ret, out, err = runCommand(cmd, env=env['ENV'])

    if ret == 0 or out or err:
        (vmaj, vmin, vpatch, buildid, versionid) = \
               parseIntelCompilerVersion(out+' '+err)
        version = '%s.%s.%s' % (vmaj, vmin, str(vpatch).rjust(3, '0'))
    else:
        version = None

    if version:
        print '** Determined Intel C/C++ Compiler Version %s' % (version,)
    else:
        version = '9.x'
        versionid = 90000
        print '** Could not determine Intel C/C++ Compiler Version, '\
              'assume %s' % (version,)

    class _version_result:
        pass

    r = _version_result()
    r.iccVersionMajor = vmaj
    r.iccVersionMinor = vmin
    r.iccVersionPatch = vpatch
    r.iccBuildId      = buildid
    r.iccVersionId    = versionid
    r.iccVersion      = version
    return r


def SetupIntelCompiler(env):
    compiler = env['compiler']
    assert compiler in ('INTEL', 'INTEL_NOGCC')
    
    print '* Setup %s compiler' % compiler

    CC = ARGUMENTS.get('CC')
    CXX = ARGUMENTS.get('CXX')

    archInfo = getArchInfo(env)
    optMode = env['optimization']
    warningsMode = env['warnings'] # False/True
    debugMode = env['debug']       # 'no' / 'symbols' / 'macro' / 'full'
    platform = env['PLATFORM']
    isWin32 = (platform == 'win32')
    msvcCompilerDir = env['msvc_compiler_dir']

    intelLinkLibIntel = env['intel_link_libintel']
    intelLinkLibGCC = env['intel_link_libgcc']

    # analyize optimization modes

    optimizeCode = (optMode not in ('none', 'debug'))

    # analyize debug modes
    debugSymbols = False
    debugMacro = False
    
    if optMode == 'debug':
        debugSymbols = True
        debugMacro = True
    elif optMode == 'none':
        debugMacro = True
        debugSymbols = False

    if debugMode == 'symbols':
        debugSymbols = True
    elif debugMode == 'macro':
        debugMacro = True
    elif debugMode == 'full':
        debugSymbols = True
        debugMacro = True

    customLinker = True
    
    if env.PropagateEnvVar('IA32ROOT'):
        env.Replace(ICCPATH = os.path.join('$IA32ROOT', 'bin'))
        ICCPATH = os.path.join(env['IA32ROOT'], 'bin')
    else:
        ICCPATH = ''

    if isWin32:
        # Win32
        #         for v in ['DevEnvDir', 'INCLUDE', 'LIB', 'LIBPATH', 'MSSdk',
        #                   'Bkoffice', 'Basemake', 'INETSDK', 'Ms']:
        #             env['ENV'] = os.environ[v]
        env['ENV'] = os.environ

        msvcInfo = setupMsvcTool(env, msvcCompilerDir, toolName='icl')

        ICLBIN = os.path.join(ICCPATH, 'icl.exe')

        if CC is None:
            CC = ICLBIN
        if CXX is None:
            CXX = ICLBIN

        if env.WhereIs(CC):
            env.Replace(CC=CC)
        else:
            print >>sys.stderr, 'Error: Could not find Intel C compiler',CC
            env.Exit(1)

        if env.WhereIs(CXX):
            env.Replace(CXX=CXX)
        else:
            print >>sys.stderr, 'Error: Could not find Intel C++ compiler',CXX
            env.Exit(1)

        iccInfo = getIntelCompilerVersion(env)

        # Deactivate Intel compiler warnings
        env.Append(CCFLAGS = split('/Qwd 1538 /Qwd 1429 /Qwd 1420 /Qwd 589 /Qwd 1572'))

        # Misc flags
        WARNING_FLAGS = ['/W1']
        DEBUG_FLAGS = ['/Zi']

        # MSVC compatibility flag
        if msvcInfo.msvcVersionId >= 90000:   # MSVC >=9.0
            env.Append(CCFLAGS = ['/Qvc9'])
        elif msvcInfo.msvcVersionId >= 80000:   # MSVC >=8.0
            env.Append(CCFLAGS = ['/Qvc8'])
        elif msvcInfo.msvcVersionId == 70000: # MSVC 7.0
            env.Append(CCFLAGS = ['/Qvc7'])
        elif msvcInfo.msvcVersionId == 70100: # MSVC 7.1
            env.Append(CCFLAGS = ['/Qvc7.1'])
        elif msvcInfo.msvcVersionId == 60000: # MSVC 6.0
            env.Append(CCFLAGS = ['/Qvc6'])

        # --------------------------------------------
        # architecture dependent flags for various processor platforms

        targetFlags = {
            CPU_PENTIUM_3 : ['/QxK'],
            CPU_PENTIUM_4 : ['/QxW'],
            CPU_PENTIUM_M : ['/QxB'],
            CPU_CORE      : ['/QxP'],
            CPU_CORE_2    : ['/QxT'],
            CPU_OPTERON   : ['/QxP'],
            CPU_ITANIUM   : ['/G2']
            }

        if optimizeCode and targetFlags.has_key(archInfo.cpu):
            env.Append(CCFLAGS = targetFlags[archInfo.cpu])
        
        # /GR : Enables C++ Run Time Type Information (RTTI).
        # /Gr : Makes __fastcall the default calling convention.
        # /O2 : Enables optimizations for speed.
        # /Ob2 : Enables inlining of any function at the compiler's discretion.
        # /Od : Disables all optimizations.
        # /TP : Tells the compiler to process all source or unrecognized file types as C++ source files.
        # /Ot : Enables all speed optimizations.
        # /Oy : Determines whether EBP is used as a general-purpose register in optimizations.
        # /G7 : Optimizes for Intel(R) Core  Duo processors, Intel(R) Core  Solo processors, Intel(R) Pentium(R) 4 processors, Intel(R) Xeon(R) processors, Intel(R) Pentium(R) M processors, and Intel(R) Pentium(R) 4 processors with Streaming SIMD Extensions 3 (SSE3) instruction support. 
        # /Zi : Tells the compiler to generate full debugging information in the object file.
        # /Og : Enables global optimizations.
        # /EHsc : enable synchronous C++ exception handling model,
        #         assume extern "C" functions do not throw exceptions
        # /MD   : Tells the linker to search for unresolved references in 
        #         a multithreaded, debug, dynamic-link run-time library. 
        # /Zp16 : Specifies alignment for structures on byte boundaries.
        # /Zc:forScope : Enforce standard behavior for initializers
        #                of for loops.
        # /Qc99 : Enables/disables C99 support for C programs. (DEPRECATED!)
        # /Gm   : enable minimal rebuild
        # /Qscalar_rep : Enables scalar replacement performed during
        #                loop transformation.
        # /Qrestrict   : Enables pointer disambiguation with the restrict
        #                qualifier. 
        # /Gd : Makes __cdecl the default calling convention.
        # /FD : Generates file dependencies related to the Microsoft* C/C++ compiler. 

        # general optimization options
        
        OPTFLAGS_NONE = split('/Od /FD /EHsc /MD /GR /Zi /Gd /Qrestrict /Qmultibyte-chars')
        OPTFLAGS_DEBUG = split('/Od /FD /EHsc /RTC1 /MDd /GR /Zi /Gd /Qrestrict /Qmultibyte-chars /vmv /vmg /GL /GF /FD /MD /GS- /arch:SSE2 /fp:fast /Zc:wchar_t- /nologo /c /TP /wd4652')
        OPTFLAGS_MEDIUM = split('/GR /O2 /Ob2 /TP /Ot /Oy /G7 /Zi /Og /EHsc /MD /Zp16 /Zc:forScope /Qscalar_rep /Qrestrict /Gd')
        OPTFLAGS_HIGH = OPTFLAGS_MEDIUM
        OPTFLAGS_EXTREME = OPTFLAGS_HIGH

        opts = {
            'none' : OPTFLAGS_NONE,
            'debug' : OPTFLAGS_DEBUG,
            'medium' : OPTFLAGS_MEDIUM,
            'high' : OPTFLAGS_HIGH,
            'extreme' : OPTFLAGS_EXTREME
            }

        if opts.has_key(optMode):
            env.Append(CCFLAGS = opts[optMode])
        else:
            # medium optimization per default
            env.Append(CCFLAGS = opts['medium'])

        # linking with/without debug
            
        if optMode == 'debug':
            env.Replace(LINKFLAGS = split('/nologo /DEBUG /MACHINE:X86 /SUBSYSTEM:CONSOLE'))
        else:
            env.Replace(LINKFLAGS = split('/nologo /MACHINE:X86 /SUBSYSTEM:CONSOLE'))

        env.Append(CPPPATH=[os.path.join('${IA32ROOT}','include')])
        
        if env.WhereIs(env['LINK']):
            customLinker = False
        else:
            customLinker = True            
            
        # end Win32
    else:
        # POSIX
        for tool in ('gcc', 'g++'):
            env.Tool(tool)

        # locate compiler

        env['ENV']['INTEL_LICENSE_FILE'] = os.environ.get('INTEL_LICENSE_FILE')

        ICCBIN = env.Detect([os.path.join(ICCPATH, 'iccbin'),
                             os.path.join(ICCPATH, 'icc')])

        if CC is None:
            CC = ICCBIN
        if CXX is None:
            CXX = ICCBIN

        if CC:
            env.Replace(CC = CC)
        else:
            print 'Error: Could not find Intel C compiler'
            env.Exit(1)

        if CXX:
            env.Replace(CXX = CXX)
        else:
            print 'Error: Could not find Intel C++ compiler'
            env.Exit(1)

        iccInfo = getIntelCompilerVersion(env)

        # Deactivate some Intel compiler warnings
        env.Append(CCFLAGS = ['-Wno-unknown-pragmas',
                              '-wd1188,1476,1505,1572,858'])

        # Setup general options
        if iccInfo.iccVersionId > 100000: # > 10.0
            OPT_CXXLIB_ICC = None
            OPT_CXXLIB_GCC = '-cxxlib'
            OPT_INLINE_LEVEL = lambda x: '-inline-level='+str(x)
            OPT_TUNE_Pentium3 = ''
            OPT_TUNE_Pentium4 = '-mtune=pentium4'
            OPT_TUNE_Itanium2 = '-mtune=itanium2'
            OPT_STATIC_INTEL = '-static-intel'
            OPT_SHARED_INTEL = '-shared-intel'
        else:
            OPT_CXXLIB_ICC = '-cxxlib-icc'
            OPT_CXXLIB_GCC = '-cxxlib-gcc'
            OPT_INLINE_LEVEL = lambda x: '-Ob'+str(x)
            OPT_TUNE_Pentium3 = '-tpp6'
            OPT_TUNE_Pentium4 = '-tpp7'
            OPT_TUNE_Itanium2 = '-tpp2'
            OPT_STATIC_INTEL = '-i-static'
            OPT_SHARED_INTEL = '-i-dynamic'

        if compiler == 'INTEL_NOGCC':
            if iccInfo.iccVersionId > 100000:
                print >>sys.stderr, 'Error: INTEL_NOGCC mode is not supported'\
                      ' by Intel C/C++ Compiler since 10.x'
                env.Exit(1)
            CXXLIB = OPT_CXXLIB_ICC
            NOGCC = '-no-gcc'
        else:
            CXXLIB = OPT_CXXLIB_GCC
            NOGCC = ''

        env.Append(CCFLAGS = [NOGCC, CXXLIB])
        
        # Misc flags
        WARNING_FLAGS = ['-w1']
        DEBUG_FLAGS = ['-g']

        # --------------------------------------------
        # architecture dependent flags for various processor platforms
        #
        # -xK == Pentium III
        # -xW or -xN == Pentium IV
        # -xB == Pentium M
        # -xP == Prescott
        # -xT == Core 2

        targetFlags = {
            CPU_PENTIUM_3 : ['-xK', OPT_TUNE_Pentium3, '-fPIC'],
            CPU_PENTIUM_4 : ['-xW', OPT_TUNE_Pentium4, '-fPIC'],
            CPU_PENTIUM_M : ['-xB', OPT_TUNE_Pentium4, '-fPIC'],
            CPU_CORE      : ['-xP', '-fPIC' ],
            CPU_CORE_2    : ['-xT', '-march=core2', '-mtune=core2', '-fPIC'],
            CPU_OPTERON   : ['-xW', '-fPIC'],
            CPU_ITANIUM   : ['-IPF_fltacc', '-mcpu=itanium2', OPT_TUNE_Itanium2, '-fPIC']
            }
        
        if optimizeCode and targetFlags.has_key(archInfo.cpu):
            env.Append(CCFLAGS = targetFlags[archInfo.cpu])

        OPTFLAGS_NONE = ['-O0', OPT_INLINE_LEVEL(0)]
        OPTFLAGS_DEBUG = ['-g'] + OPTFLAGS_NONE
        OPTFLAGS_MEDIUM = ['-O2', OPT_INLINE_LEVEL(1),'-restrict']
        OPTFLAGS_HIGH = ['-O3', OPT_INLINE_LEVEL(1), '-restrict']
        OPTFLAGS_EXTREME = ['-O3', OPT_INLINE_LEVEL(1), '-restrict', '-inline-forceinline']

        # unused:
        OPTFLAGS_ITANIUM = ['-DNDEBUG', '-restrict',
                            '-DALLOW_RESTRICT', '-O3', '-IPF_fma',
                            OPT_INLINE_LEVEL(1), '-IPF_fltacc',
                            '-IPF_flt_eval_method0',
                            '-IPF_fp_relaxed',
                            '-IPF_fp_speculationfast',
                            '-scalar_rep',
                            '-alias_args-', '-fno-alias']
        INLINE_FLAGS = ['-fno-alias', '-Qoption,c,-ip_ninl_max_stats=6000',
                        '-Qoption,c,-ip_ninl_max_total_stats=10000',
                        OPT_INLINE_LEVEL(2)]

        opts = {
            'none' : OPTFLAGS_NONE,
            'debug' : OPTFLAGS_DEBUG,
            'medium' : OPTFLAGS_MEDIUM,
            'high' : OPTFLAGS_HIGH,
            'extreme' : OPTFLAGS_EXTREME
            }

        if opts.has_key(optMode):
            env.Append(CCFLAGS = opts[optMode])
        else:
            # medium optimization per default
            env.Append(CCFLAGS = opts['medium'])

        if optimizeCode:
            env.Append(CPPDEFINES = split('ALLOW_RESTRICT'))

        # -Bsymbolic was removed from LINKFLAGS because 
        # of problems with dynamic_cast in shared libraries
        
        env.AppendUnique(LINKFLAGS = split('-rdynamic '+CXXLIB))

        if intelLinkLibIntel == 'static':
            env.AppendUnique(LINKFLAGS = [OPT_STATIC_INTEL])
        else:
            env.AppendUnique(LINKFLAGS = [OPT_SHARED_INTEL])

        if intelLinkLibGCC == 'static':
            env.AppendUnique(LINKFLAGS = ['-static-libgcc'])
        else:
            env.AppendUnique(LINKFLAGS = ['-shared-libgcc'])

        ICPCBIN = env.Detect([os.path.join(ICCPATH, 'icpcbin')])

        if ICPCBIN:
            env.Replace(ICCLINK = ICPCBIN)
        else:
            print 'Error: Could not find Intel C++ compiler (icpc)'
            env.Exit(1)

        # end POSIX

    if warningsMode:
        env.Append(CCFLAGS = WARNING_FLAGS)

    if debugSymbols:
        # produce debug symbols
        env.AppendUnique(CCFLAGS = DEBUG_FLAGS)

    if not debugMacro:
        # disable debugging macros
        env.AppendUnique(CPPDEFINES = ['NDEBUG'])

    if customLinker:
        env.Replace(LINK = '${ICCLINK}')
        
def SetupGccCompiler(env):
    print '* Setup GCC compiler'

    archInfo = getArchInfo(env)
    optMode = env['optimization']
    warningsMode = env['warnings']  # False/True
    debugMode = env['debug']        # 'no' / 'symbols' / 'output' / 'full'
    profileMode = env['profile']    # False/True
    platform = env['PLATFORM']
    isWin32 = (platform == 'win32')
    isMacOSX = (platform == 'darwin')
    
    if isWin32:
        # Workaround for the case msvc module was initialized
        # before mingw module: remove /nologo option.
        CCFLAGS = env.get('CCFLAGS')
        if CCFLAGS:
            filterOut = ['/nologo']
            CCFLAGS = filter(lambda i: i not in filterOut,
                             SCons.Util.Split(CCFLAGS))
            env.Replace(CCFLAGS=CCFLAGS)
        # Load mingw tool only if it is not already loaded
        TOOLS = env.get('TOOLS')
        if not TOOLS or 'mingw' not in TOOLS:
            env.Tool('mingw')
    elif isMacOSX:
        for tool in ('gcc', 'g++', 'applelink', 'ar', 'as'):
            env.Tool(tool)
    else:
        for tool in ('gcc', 'g++', 'gnulink', 'ar', 'as'):
            env.Tool(tool)

    # Determine CC/CXX vars (look in arguments, cache, config file(s),
    #                        environment variables)
    CACHED_OPTIONS = env['CACHED_OPTIONS']
    CONFIG_FILE_OPTIONS = env['CONFIG_FILE_OPTIONS']
        
    CACHED_OPTIONS_COMPILER = CACHED_OPTIONS.get('compiler',
                                                 GetDefaultCompilerName())
    
    if CACHED_OPTIONS_COMPILER == 'GCC':
        # cached options are for this compiler
        VAR_SEARCH_LIST = [ARGUMENTS, CONFIG_FILE_OPTIONS, CACHED_OPTIONS,
                           env['ENV'], os.environ]
    else:
        # cached options are for another compiler, ignore them
        VAR_SEARCH_LIST = [ARGUMENTS, CONFIG_FILE_OPTIONS,
                           env['ENV'], os.environ]
        print '** Cached options are for %s compiler, ignoring them.' % \
              CACHED_OPTIONS_COMPILER

    CC = findVarInDicts('CC', VAR_SEARCH_LIST, 'gcc')[0]
    CXX = findVarInDicts('CXX', VAR_SEARCH_LIST, 'g++')[0]

    # analyize optimization modes

    optimizeCode = (optMode not in ('none', 'debug'))

    # analyize debug modes
    debugSymbols = False
    debugMacro = False
    
    if optMode == 'debug':
        debugSymbols = True
        debugMacro = True
    elif optMode == 'none':
        debugMacro = True
        debugSymbols = False

    if debugMode == 'symbols':
        debugSymbols = True
    elif debugMode == 'macro':
        debugMacro = True
    elif debugMode == 'full':
        debugSymbols = True
        debugMacro = True

    # for safety update env
    env.Replace(CC = CC)
    env.Replace(CXX = CXX)
    
    # check version number
    ret, out, err = runCommand(CXX + ' --version', env=env['ENV'])

    if ret == 0 or out or err:
        (vmaj, vmin, vpatch, versionid) = parseCompilerVersion(out+' '+err)
        version = '%s.%s.%s' % (vmaj, vmin, vpatch)
    else:
        version = None

    if version:
        print '** Determined GNU C/C++ Compiler Version %s' % (version,)
    else:
        version = '4.x'
        versionid = 40000
        print '** Could not determine GNU C/C++ Compiler Version, '\
              'assume %s' % (version,)

    # --------------------------------------------
    # architecture dependent flags for various processor platforms

    targetFlags = {
        CPU_PENTIUM_3 : split('-march=pentium3 -mfpmath=sse -msse -mmmx'),
        CPU_PENTIUM_4 : split('-march=pentium4 -mfpmath=sse -msse -mmmx -msse2'),
        CPU_PENTIUM_M : split('-march=pentium-m -mfpmath=sse -msse -mmmx -msse2'),
        CPU_CORE      : split('-march=prescott -mfpmath=sse -msse -mmmx -msse2'),
        CPU_CORE_2    : split('-march=core2 -mtune=core2 -mfpmath=sse -msse -mmmx -msse2'),
        CPU_OPTERON   : split('-march=opteron -m64 -mmmx -msse -msse2'),
        CPU_ITANIUM   : split('-mtune=itanium2'),
        }

    if optimizeCode and targetFlags.has_key(archInfo.cpu):
        env.Append(CCFLAGS = targetFlags[archInfo.cpu])

    # -fPIC option
    if platform not in ('cygwin', 'win32'):
        env.Append(CCFLAGS=['-fPIC'])

    OPTFLAGS_NONE = split('-O0')
    OPTFLAGS_DEBUG = split('-g')+OPTFLAGS_NONE
    OPTFLAGS_MEDIUM = split('-O2 -finline-functions') # -fkeep-inline-functions
    OPTFLAGS_HIGH = split('-O3 -finline-functions')
    OPTFLAGS_EXTREME = OPTFLAGS_HIGH

    opts = {
        'none' : OPTFLAGS_NONE,
        'debug' : OPTFLAGS_DEBUG,
        'medium' : OPTFLAGS_MEDIUM,
        'high' : OPTFLAGS_HIGH,
        'extreme' : OPTFLAGS_EXTREME
        }

    if opts.has_key(optMode):
        env.Append(CCFLAGS = opts[optMode])
    else:
        # medium optimization per default
        env.Append(CCFLAGS = opts['medium'])

    if optimizeCode:
        env.Append(CPPDEFINES = split('ALLOW_RESTRICT'))

    if warningsMode:
        env.Append(CCFLAGS = ['-Wall', ])

    if debugSymbols:
        # produce debug symbols
        env.AppendUnique(CCFLAGS = ['-g'])

    if not debugMacro:
        # disable debugging macros
        env.AppendUnique(CPPDEFINES = ['NDEBUG'])

    if profileMode:
        env.AppendUnique(CCFLAGS = ['-g'])
        env.AppendUnique(CCFLAGS = ['-pg'])
        env.AppendUnique(LINKFLAGS = ['-pg'])

    # Deactivate GCC compiler warnings (need to do this always after -Wall)
    env.Append(CCFLAGS = '-Wno-unknown-pragmas')


def SetupMsvcCompiler(env):
    compiler = env['compiler']
    assert compiler == 'MSVC'
    
    print '* Setup %s compiler' % compiler

    CC = ARGUMENTS.get('CC')
    CXX = ARGUMENTS.get('CXX')

    archInfo = getArchInfo(env)
    optMode = env['optimization']
    warningsMode = env['warnings'] # False/True
    debugMode = env['debug']       # 'no' / 'symbols' / 'output' / 'full'
    profileMode = env['profile']   # False/True
    platform = env['PLATFORM']
    isWin32 = (platform == 'win32')
    msvcCompilerDir = env['msvc_compiler_dir']
    
    # analyize optimization modes
        
    optimizeCode = (optMode not in ('none', 'debug'))

    # analyize debug modes
    debugSymbols = False
    debugMacro = False
    
    if optMode == 'debug':
        debugSymbols = True
        debugMacro = True
    elif optMode == 'none':
        debugMacro = True
        debugSymbols = False

    if debugMode == 'symbols':
        debugSymbols = True
    elif debugMode == 'macro':
        debugMacro = True
    elif debugMode == 'full':
        debugSymbols = True
        debugMacro = True

    customLinker = True

    assert isWin32 # ??? may be later test on win64 ???
    
    env['ENV'] = os.environ

    # setup MSVC tool chain
    for tool in ('mslink', 'msvs', 'masm', 'mslib'):
        env.Tool(tool)

    setupMsvcTool(env, msvcCompilerDir, toolName='msvc')

    # Deactivate MSVC compiler warnings
    #env.Append(CCFLAGS=split('')) # /wd<n>

    # --------------------------------------------
    # architecture dependent flags for various processor platforms

    targetFlags = {
        CPU_PENTIUM_3 : [],
        CPU_PENTIUM_4 : ['/arch:SSE2'],
        CPU_PENTIUM_M : ['/arch:SSE2'],
        CPU_CORE      : ['/arch:SSE2'],
        CPU_CORE_2    : ['/arch:SSE2'],
        CPU_OPTERON   : ['/arch:SSE2'],
        CPU_ITANIUM   : []
        }

    if optimizeCode and targetFlags.has_key(archInfo.cpu):
        env.Append(CCFLAGS = targetFlags[archInfo.cpu])
    
    # /fp:fast

    OPTFLAGS_NONE = split('/Od /FD /EHsc /MD /GR /Gd')
    OPTFLAGS_DEBUG = split('/Od /FD /EHsc /RTC1 /MDd /GR /Zi /Gd /vmv /vmg')
    OPTFLAGS_MEDIUM = split('/GR /O2 /Ob2 /TP /Ot /Oy /EHsc /MD /Zp16 /Zc:forScope /Gd')
    OPTFLAGS_HIGH = OPTFLAGS_MEDIUM
    OPTFLAGS_EXTREME = OPTFLAGS_HIGH

    opts = {
        'none' : OPTFLAGS_NONE,
        'debug' : OPTFLAGS_DEBUG,
        'medium' : OPTFLAGS_MEDIUM,
        'high' : OPTFLAGS_HIGH,
        'extreme' : OPTFLAGS_EXTREME
        }

    if opts.has_key(optMode):
        env.Append(CCFLAGS = opts[optMode])
    else:
        # medium optimization per default
        env.Append(CCFLAGS = opts['medium'])

    if optimizeCode:
        env.AppendUnique(CPPDEFINES = ['NDEBUG'])

    # linking flags
    env.Replace(LINKFLAGS = split('/nologo /MACHINE:X86 /SUBSYSTEM:CONSOLE'))

    if warningsMode:
        env.AppendUnique(CCFLAGS = ['/W1'])

    if debugSymbols:
        # produce debug symbols
        env.AppendUnique(CCFLAGS = ['/Zi'])
        env.AppendUnique(LINKFLAGS = ['/DEBUG'])


RegisterCompiler('INTEL', SetupIntelCompiler)
RegisterCompiler('INTEL_NOGCC', SetupIntelCompiler)
RegisterCompiler('GCC', SetupGccCompiler)
RegisterCompiler('MSVC', SetupMsvcCompiler)
SetDefaultCompilerName('GCC')

# Architectures

# archSuffixDict dictionary map between scons's platform and 
# architecture suffix def. If scons's platform is not in the dict, 
# 'default' mapping will be used instead.

ARCH_SUFFIX_DICT = {
    'default' : 'POSIX',
    'win32' : 'WINDOWS',
    'os2' : 'OS2',
    'darwin' : 'MACOSX'
    }

def SetupArch(env):
    global ARCH_SUFFIX_DICT

    arch = env['arch']
    platform = env['PLATFORM']
    staticlibs = env['staticlibs']
    projPrefix = getProjectPrefix(env)
    archPrefix = getProjectSystemPrefix(env)
    
    print '* Setup %s architecture on platform %s' % (arch, platform)

    platformDef = archPrefix + ARCH_SUFFIX_DICT.get(platform, 
                                                    ARCH_SUFFIX_DICT['default'])
    if not env.AddConfigKey(platformDef, 1):
        env.Append(CPPDEFINES=[platformDef])

    if staticlibs:
        if not env.AddConfigKey(key, 1):
            env.Append(CPPDEFINES=[key])

    archInfo = getArchInfo(env)

    if archInfo.arch == ARCH_X86:
        env.Replace(TARGET_ARCH_SUFFIX = 'IA32') #??? change it to X86
        env.Append(CPPDEFINES = ['IA32'])          # remove ?
    elif archInfo.arch == ARCH_X86_64:
        env.Replace(TARGET_ARCH_SUFFIX = 'X64')  #??? change it to X86_64
        env.Append(CPPDEFINES = ['ARCH_ATHLON64']) # remove ?
        
        # Check for INTEL Compiler as combination of INTEL/Opteron
        # causes problems when including C++'s <cmath> header file
        if env['compiler'] in ('INTEL', 'INTEL_NOGCC'):
            key = projPrefix+'USE_MATH_H'
            if not env.AddConfigKey(key, 1):
                env.Append(CPPDEFINES=[key])

        # Check for SuSE system, as it causes problems
        # on Opterons
        if os.path.exists('/etc/SuSE-release'):
            fd = open('/etc/SuSE-release', 'r')
            content = fd.read()
            fd.close()
            if 'X86-64' in content:
                print '*** Warning: SuSE X86-64 system found, -i-static disabled'
                env.Replace(LINKFLAGS = filterOut('-i-static', env['LINKFLAGS']))
            del fd
            del content
    elif archInfo.arch == ARCH_IA64:
        env.Replace(TARGET_ARCH_SUFFIX = 'IA64')
        env.Append(CPPDEFINES = ['ARCH_ITANIUM'])  # remove ?
        if env['compiler'] == 'INTEL_NOGCC':
            env.Append(LINKFLAGS = split('-static-libcxa -rdynamic -i-static'))

for a in ('x86', 'x86_64', 'PentiumIII', 'PentiumIV', 'PentiumM', 'Core2',
          'Opteron', 'Itanium'):
    RegisterArchitecture(a, SetupArch)

for a in ('i686', 'i586', 'i386'):
    RegisterArchitectureAlias(a, 'x86')

SetDefaultArchitectureName('x86')

# Load all additional configuration modules

loadModules(os.path.join(os.path.dirname(__file__), 'modules'))
