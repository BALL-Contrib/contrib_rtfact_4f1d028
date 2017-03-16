/* 
 *  btools.py
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
 
 # SCons Builder Engine Tools V2.1

import os
import os.path
import re
import fnmatch
import string
import copy
import SCons
import SCons.Util
import SCons.Errors
from types import IntType
from SCons.Script import \
     Environment, AddMethod, ARGUMENTS, Configure, \
     BoolOption, EnumOption, ListOption, PackageOption, PathOption

######################### Utilities #########################

# Portions from SCons Environment.Append

def _appendVars(destDict, srcDict):
    for key, val in srcDict.items():
        # It would be easier on the eyes to write this using
        # "continue" statements whenever we finish processing an item,
        # but Python 1.5.2 apparently doesn't let you use "continue"
        # within try:-except: blocks, so we have to nest our code.
        try:
            orig = destDict[key]
        except KeyError:
            # No existing variable in the environment, so just set
            # it to the new value.
            destDict[key] = val
        else:
            try:
                # Check if the original looks like a dictionary.
                # If it is, we can't just try adding the value because
                # dictionaries don't have __add__() methods, and
                # things like UserList will incorrectly coerce the
                # original dict to a list (which we don't want).
                update_dict = orig.update
            except AttributeError:
                try:
                    # Most straightforward:  just try to add them
                    # together.  This will work in most cases, when the
                    # original and new values are of compatible types.
                    destDict[key] = orig + val
                except (KeyError, TypeError):
                    try:
                        # Check if the original is a list.
                        add_to_orig = orig.append
                    except AttributeError:
                        # The original isn't a list, but the new
                        # value is (by process of elimination),
                        # so insert the original in the new value
                        # (if there's one to insert) and replace
                        # the variable with it.
                        if orig:
                            val.insert(0, orig)
                        destDict[key] = val
                    else:
                        # The original is a list, so append the new
                        # value to it (if there's a value to append).
                        if val:
                            add_to_orig(val)
            else:
                # The original looks like a dictionary, so update it
                # based on what we think the value looks like.
                if SCons.Util.is_List(val):
                    for v in val:
                        orig[v] = None
                else:
                    try:
                        update_dict(val)
                    except (AttributeError, TypeError, ValueError):
                        if SCons.Util.is_Dict(val):
                            for k, v in val.items():
                                orig[k] = v
                        else:
                            orig[val] = None

# Portions from SCons Environment.MergeFlags

def _mergeVars(destDict, srcDict, unique=1):
    if not unique:
        _appendVars(destDict, srcDict)
        return destDict
    for key, value in srcDict.items():
        if not value:
            continue
        try:
            orig = destDict[key]
        except KeyError:
            orig = value
        else:
            if not orig:
                orig = value
            elif value:
                # Add orig and value.  The logic here was lifted from
                # part of env.Append() (see there for a lot of comments
                # about the order in which things are tried) and is
                # used mainly to handle coercion of strings to CLVar to
                # "do the right thing" given (e.g.) an original CCFLAGS
                # string variable like '-pipe -Wall'.
                try:
                    orig = orig + value
                except (KeyError, TypeError):
                    try:
                        add_to_orig = orig.append
                    except AttributeError:
                        value.insert(0, orig)
                        orig = value
                    else:
                        add_to_orig(value)
        t = []
        if key[-4:] == 'PATH':
            ### keep left-most occurence
            for v in orig:
                if v not in t:
                    t.append(v)
        else:
            ### keep right-most occurence
            orig.reverse()
            for v in orig:
                if v not in t:
                    t.insert(0, v)
        destDict[key] = t
    return destDict

# ??? Use SCons.Util.Split instead of toList ???
def toList(arg):
    if SCons.Util.is_String(arg):
        return string.split(arg)
    else:
        return list(arg)

def findVarInDicts(name, dictSeq, defaultValue=None):
    """returns tuple (value, dict) on success, (defaultValue, None) otherwise
    """
    for d in dictSeq:
        if d.has_key(name):
            return (d[name], d)
    return (defaultValue, None)

# Slightly modified version of backtick function
# from SCons's engine/SCons/Environment.py

def runCommand(command, env=None):
    """returns triple (returncode, stdout, stderr)"""
    import subprocess
    myenv = {}
    for k, v in env.items():
        myenv[str(k)] = str(v)
    env = myenv
    if SCons.Util.is_List(command):
        p = subprocess.Popen(command,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE,
                             env=env,
                             universal_newlines=False)
    else:
        p = subprocess.Popen(command,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE,
                             env=env,
                             universal_newlines=False,
                             shell=True)
    out = p.stdout.read()
    p.stdout.close()
    err = p.stderr.read()
    p.stderr.close()
    status = p.wait()
    
    return (status, out, err)

def scIsAbsPath(path):
    """SCons specific way to check for absolute path"""
    return path.startswith('#') or os.path.isabs(path)

def scAbsPath(path):
    """SCons specifc way to make absolute paths.
       This version does not require environment
    """
    if not scIsAbsPath(path):
        path = '#'+path
    return path


######################### Builders #########################

# ConfigH builder, code from SCons v0.97

def _createConfigH(target, source, env):
    t = open(str(target[0]), "w")
    defname = re.sub('[^A-Za-z0-9_]', '_', (str(target[0])).upper())
    t.write("""#ifndef %(DEFNAME)s_SEEN
#define %(DEFNAME)s_SEEN

""" % {'DEFNAME' : defname})
    t.write(source[0].get_contents())
    t.write("""
#endif /* %(DEFNAME)s_SEEN */
""" % {'DEFNAME' : defname})
    t.close()

def _stringConfigH(target, source, env):
    return 'Generating C/C++ config file ' + str(target[0])

def CreateConfigHBuilder(env):
    action = SCons.Action.Action(_createConfigH, _stringConfigH)
    env['BUILDERS']['ConfigHBuilder'] = env.Builder(action=action)

######################### Helpers #########################

def DumpEnv(env, key = None, header = None, footer = None):
    """
    Using the standard Python pretty printer, dump the contents of the
    scons build environment to stdout.
 
    If the key passed in is anything other than 'env', then that will
    be used as an index into the build environment dictionary and
    whatever is found there will be fed into the pretty printer. Note
    that this key is case sensitive.
 
    The header and footer are simple mechanisms to allow printing a
    prefix and suffix to the contents that are dumped out. They are
    handy when using DumpEnv to dump multiple portions of the
    environment.
    """
    import pprint
    pp = pprint.PrettyPrinter( indent = 2 )
    if key:
        dict = env.Dictionary( key )
    else:
        dict = env.Dictionary()
    if header:
        print header
    pp.pprint( dict )
    if footer:
        print footer

class DistGroup:

    def __init__(self, name):
        self.name = name
        self.files = []

    def addFile(self, fileNode, distName):
        self.files.append((fileNode, distName))

    def copyInfos(self, fileList, nameDict):
        for (f, n) in self.files:
            fileList.append(f)
            if n is not None:
                nameDict[f.srcnode().abspath] = n

# Registry stores data which are independent from
# one specific environment instance

class Registry:
    
    def __init__(self):
        self.compilers = {}
        self.defaultCompiler = None
        self.architectures = {}
        self.archNameList = []
        self.archAliases = {}
        self.defaultArchitecture = None
        self.customTests = {}
        self.customOptions = []
        self.envCustomizers = []
        self.envFinalizers = []
        self.configCustomizers = []
        self.distGroups = {}

    def setDefaultCompilerName(self, name):
        self.defaultCompiler = name

    def setDefaultArchitectureName(self, name):
        self.defaultArchitecture = self.resolveArchName(name)

    def getDefaultCompilerName(self):
        return self.defaultCompiler

    def getDefaultArchitectureName(self):
        return self.defaultArchitecture

    def registerCompiler(self, name, setupFunc):
        self.compilers[name] = setupFunc

    def registerArchitecture(self, name, setupFunc):
        self.architectures[name] = setupFunc
        if name not in self.archNameList:
            self.archNameList.append(name)

    def registerArchitectureAlias(self, aliasName, archName):
        self.archAliases[aliasName] = archName

    def resolveArchName(self, name):
        i = 0
        while i < 100: # maximally 100 lookups
            n = self.archAliases.get(name)
            i+=1
            if n == None:
                break
            name = n
        return name

    def getCompiler(self, name):
        return self.compilers.get(name)

    def getArchitecture(self, name):
        return self.architectures.get(self.resolveArchName(name))

    def isArchitectureSupported(self, name):
        return self.architectures.has_key(self.resolveArchName(name))

    def getCompilerNames(self):
        return self.compilers.keys()

    def getArchitectureNames(self):
        return self.archNameList[:]

    def getCustomTests(self):
        return self.customTests

    def registerCustomTest(self, name, testFunc):
        self.customTests[name] = testFunc

    def _addCustomOptionImpl(self, key, help="", default=None, validator=None, converter=None):
        self.customOptions.append((key, help, default, validator, converter))

    def addCustomOption(self, key, help="", default=None, validator=None, converter=None, **kw):
        """Accepts same arguments as Options.Add"""
        if SCons.Util.is_List(key) or type(key) == type(()):
            apply(self._addCustomOptionImpl, key)
            return

        if not SCons.Util.is_String(key) or \
           not SCons.Util.is_valid_construction_var(key):
            raise SCons.Errors.UserError, "Illegal Options.Add() key `%s'" % str(key)

        self._addCustomOptionImpl(key, help, default, validator, converter)


    def addCustomOptions(self, *optlist):
        """Accepts same arguments as Options.AddOptions"""
        for o in optlist:
            apply(self._addCustomOptionImpl, o)

    def getCustomOptions(self):
        return self.customOptions

    def registerEnvironmentCustomizer(self, func):
        self.envCustomizers.append(func)

    def customizeEnvironment(self, env):
        for envCustomizer in self.envCustomizers:
            envCustomizer(env)

    def registerEnvironmentFinalizer(self, func):
        self.envFinalizers.append(func)

    def finalizeEnvironment(self, env):
        for envFinalizer in self.envFinalizers:
            envFinalizer(env)

    def registerConfigurationCustomizer(self, func):
        self.configCustomizers.append(func)

    def customizeConfiguration(self, conf):
        for configCustomizer in self.configCustomizers:
            configCustomizer(conf)

    def addDistFile(self, fileNode, distName, distGroupName = None):
        if distGroupName is None:
            distGroupName = 'default'
        distGroup = self.distGroups.get(distGroupName)
        if distGroup is None:
            distGroup = DistGroup(distGroupName)
            self.distGroups[distGroupName] = distGroup
        distGroup.addFile(fileNode, distName)

    def copyDistInfos(self, distGroupName, fileList, nameDict):
        distGroup = self.distGroups.get(distGroupName)
        if distGroup:
            distGroup.copyInfos(fileList, nameDict)

registry = Registry()
RegisterCompiler = registry.registerCompiler
RegisterArchitecture = registry.registerArchitecture
RegisterArchitectureAlias = registry.registerArchitectureAlias
GetCompiler = registry.getCompiler
GetArchitecture = registry.getArchitecture
IsArchitectureSupported = registry.isArchitectureSupported
GetCompilerNames = registry.getCompilerNames
GetArchitectureNames = registry.getArchitectureNames
SetDefaultCompilerName = registry.setDefaultCompilerName
SetDefaultArchitectureName = registry.setDefaultArchitectureName
GetDefaultCompilerName = registry.getDefaultCompilerName
GetDefaultArchitectureName = registry.getDefaultArchitectureName
GetCustomTests = registry.getCustomTests
RegisterCustomTest = registry.registerCustomTest
AddCustomOption = registry.addCustomOption
AddCustomOptions = registry.addCustomOptions
GetCustomOptions = registry.getCustomOptions
RegisterEnvironmentCustomizer = registry.registerEnvironmentCustomizer
CustomizeEnvironment = registry.customizeEnvironment
RegisterEnvironmentFinalizer = registry.registerEnvironmentFinalizer
FinalizeEnvironment = registry.finalizeEnvironment
RegisterConfigurationCustomizer = registry.registerConfigurationCustomizer
CustomizeConfiguration = registry.customizeConfiguration
AddDistFile = registry.addDistFile

######################### Package Support #########################

class Package:

    VARS_TO_SAVE = [
        'ASFLAGS',
        'CFLAGS',
        'CCFLAGS',
        'CPPDEFINES',
        'CPPFLAGS',
        'CPPPATH',
        'FRAMEWORKPATH',
        'FRAMEWORKS',
        'LIBPATH',
        'LIBS',
        'LINKFLAGS',
        'RPATH' ]

    def __init__(self, name, vars=None, dependencies=None, **kw):
        self.name = str(name)

        self.vars = {}
        if vars is not None:
            if not SCons.Util.is_Dict(vars):
                raise SCons.Errors.UserError, "Illegal vars `%s' argument for Package.__init__(), dictionary expected" % str(vars)

            self.vars.update(vars)
        self.vars.update(kw)

        if dependencies is None:
            self.dependencies = []
        else:
            self.dependencies = SCons.Util.Split(dependencies)

    def getName(self):
        return self.name

    def addDependencies(self, dependencies):
        self.dependencies.extend(SCons.Util.Split(dependencies))

    def _getPackageDependenciesRec(self, packages, packageDeps, seenPackages,
                                   packageStack, addSelfDependency=True):
        """Recursively get all package dependencies into packageDeps

           packages - dictionary with all available packages
           packageDeps  - list where to put all package dependencies
           seenPackages - list of packages which were already visited
           packageStack - stack of packages used for checking of circular 
                          dependencies
           addSelfDependency - if true this package will be also added
                               to the dependency list
        """
        if self in packageStack:
            print "Warning: circular dependency for package `%s'" % self.name
            print '%s' % self.name,
            print 'in',map(lambda p: p.name, packageStack)
            return
        packageStack.append(self)
        try:
            # don't visit twice this package
            if self in seenPackages:
                return
            seenPackages.append(self)

            # process all dependencies
            for dep in self.dependencies:
                if isinstance(dep, Package):
                    depName = dep.getName()
                else:
                    depName = str(dep)

                try:
                    package = packages[depName]
                except KeyError:
                    raise SCons.Errors.UserError, "Unknown dependency package `%s' in package `%s'" % (depName, self.name)

                # use depth-first search to get all packages on which
                # pkg depends.
                package._getPackageDependenciesRec(packages, 
                                                   packageDeps, 
                                                   seenPackages,
                                                   packageStack)

            # finally put this package to the dependencies list
            if addSelfDependency:
                packageDeps.append(self)
        finally:
            packageStack.pop()

    def _getPackageDependencies(self, env):
        packages = env['SB_PACKAGES']
        packageDeps = []
        seenPackages = []
        packageStack = []
        self._getPackageDependenciesRec(packages, packageDeps, seenPackages,
                                        packageStack,
                                        addSelfDependency=False)
        return packageDeps

    def getModifiedVarNames(self, env):
        packageDeps = self._getPackageDependencies(env)
        
        names = self.vars.keys()
        for p in packageDeps:
            for k in p.vars.keys():
                if k not in names:
                    names.append(k)
        return names

    def saveVars(self, env):
        varNames = self.getModifiedVarNames(env)
        return env.SaveVars(varNames)
        
    def updateEnv(self, env, unique=1):
        packageDeps = self._getPackageDependencies(env)

        for p in packageDeps:
            env.MergeFlags(p.vars)

        env.MergeFlags(self.vars)

        return env

    def updateVars(self, env, vars, unique=1):
        packageDeps = self._getPackageDependencies(env)

        for p in packageDeps:
            _mergeVars(vars, p.vars)

        _mergeVars(vars, self.vars)

        return vars
    
    def __repr__(self):
        return 'Package(%s, vars=%s, dependencies=%s)' % \
            (repr(self.name), repr(self.vars), repr(self.dependencies))

def DeclarePackage(env, name, vars=None, dependencies=None, 
                   trigger_libs=None, trigger_frameworks=None,
                   **kw):
    """Declares new compilation package.
       name - package name, arbitrary names could be used, however nameing
              convention is to use lower case only
       vars - environment variables dictionary which specify all options
              that must be appended to the environment in order to
              link with the package
       dependencies - names of all dependend packages, they will be linked
                      before this package
       trigger_libs - names of libraries which will trigger this package
       trigger_frameworks - names of frameworks which will trigger this package
       kw   - keyword arguments which are equivalent to the vars
    """
    envDict = env.Dictionary()
    if not envDict.has_key('SB_PACKAGES'):
        orig = {}
        envDict['SB_PACKAGES'] = orig
    else:
        orig = envDict['SB_PACKAGES']
    if orig.has_key(name):
        raise SCons.Errors.UserError, "Package `%s' already declared" % str(name)
    package = Package(name, vars, dependencies, **kw)
    envDict['SB_PACKAGES'][name] = package
    AddPackageTrigger(env, package, 
                      trigger_libs=trigger_libs, 
                      trigger_frameworks=trigger_frameworks)
    return package

def GetPackage(env, name):
    try:
        return env['SB_PACKAGES'][name]
    except KeyError:
        return None

def AddPackageTrigger(env, package, vars=None, 
                      trigger_libs=None, trigger_frameworks=None):

    if SCons.Util.is_String(package):
        p = GetPackage(env, package)
        if not p:
            raise SCons.Errors.UserError, "Undefined package `%s'" % \
                str(package)
        package = p

    if trigger_libs is None:
        trigger_libs = []
    else:
        trigger_libs = SCons.Util.Split(trigger_libs)

    if trigger_frameworks is None:
        trigger_frameworks = []
    else:
        trigger_frameworks = SCons.Util.Split(trigger_frameworks)

    if vars is not None:
        try:
            LIBS = vars['LIBS']
            trigger_libs.extend(LIBS)
        except KeyError:
            pass
        try:
            FRAMEWORKS = vars['FRAMEWORKS']
            trigger_frameworks.extend(FRAMEWORKS)
        except KeyError:
            pass

    if len(trigger_libs) == 0 and len(trigger_frameworks) == 0:
        return

    envDict = env.Dictionary()
    if not envDict.has_key('SB_PACKAGE_TRIGGERS'):
        orig = {'LIBS' : {}, 'FRAMEWORKS' : {}}
        envDict['SB_PACKAGE_TRIGGERS'] = orig
    else:
        orig = envDict['SB_PACKAGE_TRIGGERS']

    for lib in trigger_libs:
        if not orig['LIBS'].has_key(lib):
            orig['LIBS'][lib] = package

    for frm in trigger_frameworks:
        if not orig['FRAMEWORKS'].has_key(frm):
            orig['FRAMEWORKS'][frm] = package
        
    envDict['SB_PACKAGE_TRIGGERS'] = orig

def RequirePackage(env, names):
    """Configures environment variables for compiling with in name
       argument specified packages.
       Returns dictionary of orignal values of all modified environment
       variables."""
    # create temporary package for representing a list
    # of required dependencies
    
    tmpPkg = Package('__tmp', dependencies=names)
    vars = {}
    vars.update(tmpPkg.saveVars(env))
    tmpPkg.updateEnv(env)
    del tmpPkg
    return vars

######################### Misc #########################

def GetArchitectureId():
    """returns one of x86,x86_64,??? or empty string"""
    # check architecture
    machine = ''
    try:
        import platform
        machine = platform.machine()
    except ImportError:
        uname = getattr(os, 'uname', None)
        if uname != None:
            machine = uname()[4]
    return machine

def _installEnvironmentMethods(env):
    env.AddMethod(InstallFiles, 'InstallFiles')
    env.AddMethod(PublicHeaders, 'PublicHeaders')
    env.AddMethod(PublicBinaries, 'PublicBinaries')
    env.AddMethod(DistributeFiles, 'DistributeFiles')
    env.AddMethod(CreateDist, 'CreateDist')
    env.AddMethod(DumpEnv, 'DumpEnv')
    
    env.AddMethod(DefaultLibrary, 'DefaultLibrary')
    env.AddMethod(_BuilderWrapper(_BuilderWrapper.STATIC_LIBRARY),
                  'SB_StaticLibrary')
    env.AddMethod(_BuilderWrapper(_BuilderWrapper.SHARED_LIBRARY),
                  'SB_SharedLibrary')
    env.AddMethod(_BuilderWrapper(_BuilderWrapper.LOADABLE_MODULE),
                  'SB_LoadableModule')
    env.AddMethod(_BuilderWrapper(_BuilderWrapper.PROGRAM),
                  'SB_Program')

    env.AddMethod(GetLibraryDir, 'GetLibraryDir')
    env.AddMethod(FindEnvVar, 'FindEnvVar')
    env.AddMethod(GetEnvVar, 'GetEnvVar')
    env.AddMethod(PropagateEnvVar, 'PropagateEnvVar')
    env.AddMethod(GlobalLib, 'GlobalLib')
    env.AddMethod(GlobalBinary, 'GlobalBinary')
    env.AddMethod(GetRealPath, 'GetRealPath')
    env.AddMethod(SourceGlob, 'SourceGlob')
    env.AddMethod(SourceGlobExpand, 'SourceGlobExpand')
    env.AddMethod(GetRootDir, 'GetRootDir')
    env.AddMethod(GetLibDir, 'GetLibDir')
    env.AddMethod(GetIncludeDir, 'GetIncludeDir')
    env.AddMethod(FilterFiles, 'FilterFiles')
    env.AddMethod(FilterOut, 'FilterOut')
    env.AddMethod(AddPrefix, 'AddPrefix')
    env.AddMethod(PhonyTarget, 'PhonyTarget')
    env.AddMethod(CreateConfigHeader, 'CreateConfigHeader')
    env.AddMethod(GetConfigHeader, 'GetConfigHeader')
    env.AddMethod(GenerateConfigHeader, 'GenerateConfigHeader')
    env.AddMethod(AddConfigKeyEnv, 'AddConfigKey')

def SetupEnvironment(env):
    # install initial required methods
    _installEnvironmentMethods(env)
    
    # load required tools
    global __file__
    
    toolpath = [os.path.join(os.path.dirname(__file__), 'scons_tools')]
    env.Tool('disttar', toolpath = toolpath)

    CreateConfigHBuilder(env)
    
    # setup environment
    env.Replace(SB_CREATED_LIBRARIES={})
    env.Replace(ARGUMENTS=ARGUMENTS)
    env.Replace(SB_CONFIG_HS={})
    env.Replace(SB_CURRENT_CONFIG_H=None)

    # if specified initialize config header
    configHeader = env.get('CONFIG_HEADER')
    if SCons.Util.is_String(configHeader):
        configHeader = ConfigHeader(filename=configHeader)
    elif isinstance(configHeader, ConfigHeader):
        pass
    elif configHeader:
        configHeader = ConfigHeader()
    else:
        configHeader = None
    env.Replace(CONFIG_HEADER = configHeader)
    
    # setup compiler
    compilerSetupFunc = GetCompiler(env.get('compiler'))
    if compilerSetupFunc:
        compilerSetupFunc(env)
    
    # setup user-defined compiler options:
    # libpath   -> LIBPATH
    # cpppath   -> CPPPATH
    # ccflags   -> CCFLAGS
    # linkflags -> LINKFLAGS

    if env.get('libpath'):
        libpath = env['libpath'].split(os.pathsep)
        for p in libpath:
            env.Append(LIBPATH=[p])

    if env.get('cpppath'):
        cpppath = env['cpppath'].split(os.pathsep)
        for p in cpppath:
            env.Append(CPPPATH=[p])

    if env.get('ccflags'):
        ccflags = env['ccflags'].split()
        for f in ccflags:
            env.Append(CCFLAGS=[f])

    if env.get('linkflags'):
        linkflags = env['linkflags'].split()
        for f in linkflags:
            env.Append(LINKFLAGS=[f])
        
    # setup architecture
    archSetupFunc = GetArchitecture(env.get('arch'))
    if archSetupFunc:
        archSetupFunc(env)

    CustomizeEnvironment(env)

    # setup configuration

    conf = Configure(env, custom_tests=GetCustomTests(),
                     conf_dir=os.path.join(env['BUILD_DIR'], '.sconf_temp'),
                     log_file=os.path.join(env['BUILD_DIR'], 'config.log'))

    setattr(conf, 'sb_config_h', configHeader)

    CustomizeConfiguration(conf)

    env = conf.Finish()

    # setup modified versions of the Program and Library builders

    # save original builders
    env.scStaticLibrary = env.StaticLibrary
    env.scSharedLibrary = env.SharedLibrary
    env.scProgram = env.Program
    env.scLibrary = env.Library
    env.scLoadableModule = env.LoadableModule
    
    # install my builders
    env.StaticLibrary = env.SB_StaticLibrary
    env.Library = env.SB_StaticLibrary
    env.SharedLibrary = env.SB_SharedLibrary
    env.LoadableModule =  env.SB_LoadableModule
    env.Program = env.SB_Program

    FinalizeEnvironment(env)

    return env

def GetRealPath(env, fileName):
    if not isinstance(fileName, str):
        return fileName
    
    ## get real path, not build path
    if not scIsAbsPath(fileName):
        return os.path.join(env.Dir('.').srcnode().abspath, fileName)
    else:
        # absolute
        return fileName

def GetRealPaths(env, fileNames):
    if isinstance(fileNames, str):
        return GetRealPath(env, fileNames)
    else:
        return map(lambda fn: GetRealPath(env, fn), fileNames)

def processPathVar(env, kw, name, prepend=None):
    val = GetRealPaths(env, toList(kw.get(name, [])))
    pl = []
    if isinstance(prepend, str):
        pl.append(prepend)
    elif isinstance(prepend, list) or isinstance(prepend, tuple):
        pl = list(prepend)
    kw[name] = pl + env[name] + val

def processVar(env, kw, name):
    val = toList(kw.get(name, []))
    kw[name] = env[name] + val

def processCPPDEFINES(env, kw):
    return processVar(env, kw, 'CPPDEFINES')

def processCPPPATH(env, kw):
    return processPathVar(env, kw, 'CPPPATH', prepend='.')

def processLIBPATH(env, kw):
    return processPathVar(env, kw, 'LIBPATH')

# _BuilderWrapper is used to replace StaticLibrary, SharedLibrary,
# LoadableModule, Library and Program built-ins builders of SCons.
# Only in the configuration step original SCons builders are accessible.
# The wrappers provide an ability to avoid specification of LIBPATH
# for libraries created inside of a project. LIBPATH should be now only
# specified for external libraries.
# Wrappers are also directly accessible with the names prefixed by "SB_":
# SB_StaticLibrary, SB_SharedLibrary, etc.
#
# After configuration step originals are saved in the SCons environment with
# "sc" prefix (see SetupEnvironment) and are replaced by the wrappers.

class _BuilderWrapper:

    # wrapper generation mode

    PROGRAM          = 0
    SHARED_LIBRARY   = 1
    STATIC_LIBRARY   = 2
    LOADABLE_MODULE  = 3

    def __init__(self, mode):
        self.mode = mode
        
    def getBuilder(self, env):
        if self.mode == self.PROGRAM:
            return env.scProgram
        elif self.mode == self.SHARED_LIBRARY:
            return env.scSharedLibrary
        elif self.mode == self.STATIC_LIBRARY:
            return env.scStaticLibrary
        elif self.mode == self.LOADABLE_MODULE:
            return env.scLoadableModule
        raise SCons.Errors.UserError, \
              "Illegal BuilderWrapper mode %i" % self.mode

    def __call__(self,  env, *args, **kw):
        # target is global per default

        if self.mode == self.PROGRAM:
            targetMod = lambda binName: env.GlobalBinary(binName)
        else:
            # library
            targetMod = lambda libName: env.GlobalLib(libName)

        if kw.has_key('globalTarget'):
            if not kw['globalTarget']:
                targetMod = lambda libName: libName
            del kw['globalTarget']

        if kw.has_key('target'):
            target = targetMod(kw['target'])
            # write back
            kw['target'] = target
        elif args:
            # args will be modified, so convert them to the list
            args = list(args)
            target = targetMod(args[0])
            # write back
            args[0] = target

        # process triggers

        # remove any as trigger registered libraries and frameworks
        # and insert corresponding packages into the packages list
        packages = []
        packageTriggers = env['SB_PACKAGE_TRIGGERS']

        if not SCons.Util.is_Dict(packageTriggers):
            raise SCons.Errors.UserError, "Internal Error: env['SB_PACKAGE_TRIGGERS']=%s is not a dictionary" % repr(packageTriggers)

        for triggerKey, triggers in packageTriggers.items():
            if kw.has_key(triggerKey):
                items = kw[triggerKey]
                # items is LIBS / FRAMEWORKS list, and because elements
                # can be variables $VAR we need to expand them first
                expandedItems = map(env.subst, items)

                assert len(items) == len(expandedItems)
                newItems = []
                for i in xrange(len(items)):
                    if triggers.has_key(items[i]):
                        # duplicate packages will be removed later
                        packages.append(triggers[items[i]])
                    elif triggers.has_key(expandedItems[i]):
                        # duplicate packages will be removed later
                        packages.append(triggers[expandedItems[i]])
                    else:
                        newItems.append(items[i])
                kw[triggerKey] = newItems

        # apply packages by creating of a temporary package with
        # packages list as dependencies and modifying kw dictionary
        tmpPkg = Package('__tmp', dependencies=packages)
        tmpPkg.updateVars(env, kw)
        del tmpPkg
        del packages

        processCPPPATH(env, kw)
        processLIBPATH(env, kw)
        processCPPDEFINES(env, kw)

        build = self.getBuilder(env)(*args, **kw)
        
        if target and self.mode != self.PROGRAM:
            # register created library and add its path to the LIBPATH variable
            env['SB_CREATED_LIBRARIES'][target] = build[0]

            env.AppendUnique(LIBPATH=[
                scAbsPath(os.path.dirname(build[0].path))])
            
        return build

def DefaultLibrary(env, *args, **kw):
    if env.get('staticlibs', 0):
        return env.StaticLibrary(*args, **kw)
    else:
        return env.SharedLibrary(*args, **kw)

def GetLibraryDir(env, libName):
    lib = env['SB_CREATED_LIBRARIES'].get(libName)
    if not lib:
        # try global library libName
        lib = env['SB_CREATED_LIBRARIES'].get(env.GlobalLib(libName))
        
    if lib:
        return scAbsPath(os.path.dirname(lib.path))
    else:
        return None

def FindEnvVar(env, name, defaultValue=None):
    """env is SCons Environment class
       name is variable name
    """
    # resolution order :
    # 1. command line arguments
    # 2. SCons environment
    # 3. SCons ENV environment variable used as OS
    #    environment for command execution
    # 4. os.environ
    return findVarInDicts(name, [ARGUMENTS, env, env['ENV'], os.environ])

def GetEnvVar(env, name, defaultValue=None):
    """env is SCons Environment class
       name is variable name
    """
    rval, rdict = FindEnvVar(env, name, defaultValue=defaultValue)
    return rval

def PropagateEnvVar(env, name, defaultValue=None):
    """if option <name> is in env and return it,
       if option <name> is in env['ENV'] set it in the env and return it,
       otherwise return defaultValue"""
    # first look in env, then in command environment
    if env.has_key(name):
        return env[name]
    cmdenv = env['ENV']
    if cmdenv.has_key(name):
        val = cmdenv[name]
        env.Replace(**{name : val})
        return val
    if os.environ.has_key(name):
        val = os.environ[name]
        env.Replace(**{name : val})
        return val
    return defaultValue

def SaveVars(env, vars):
    """Clone and return dictionary of all variables specified in vars list"""
    vars = SCons.Util.Split(vars)
    r = {}
    for v in vars:
        try:
            r[v] = copy.deepcopy(env[v])
        except KeyError:
            pass
    return r

def RestoreVars(env, vars):
    """Copy all variables from vars dictionary to environment"""
    if not SCons.Util.is_Dict(vars):
        raise SCons.Errors.UserError, "Illegal vars `%s' argument to RestoreVars, dictionary expected" % str(vars)
    for k,v in vars.items():
        env[k] = v

def GlobalLib(env, libName):
    if not scIsAbsPath(libName):
        return os.path.join(env['LIB_DIR'], libName)
    else:
        return libName

def GlobalBinary(env, binName):
    if not scIsAbsPath(binName):
        return os.path.join(env['BIN_DIR'], binName)
    else:
        return binName

def SourceGlob(env, pat):
    ## GLOB IN THE REAL SOURCE DIRECTORY (NOT BUILD DIR)
    import glob
    prevdir = os.getcwd()
    if pat[0] != '#':
        os.chdir(env.Dir('.').srcnode().abspath)
        ret = glob.glob(pat)
    else:
        pat = pat[1:]
        base = os.path.dirname(pat)
        searchdir = env.Dir('#').srcnode().abspath
        os.chdir(searchdir)
        ret = ['#'+x for x in glob.glob(pat)]
    os.chdir(prevdir)
    return ret

def SourceGlobExpand(env, list):
    ## look for pattern-like things and glob on those
    ret = []
    for item in list:
        if SCons.Util.is_String(item):
            if item.find('*') != -1 or \
                   item.find('?') != -1 or \
                   item.find('[') != -1:
                ret += SourceGlob(env, item)
            else:
                ret += [GetRealPath(env,item)]
        else:
            ret.append(item)
    return ret

def InstallFiles(env, targetDir, source):
    if isinstance(source, str):
        source = env.Split(source)

    source = SourceGlobExpand(env, source)
    nodes = []
    
    for i in source:
        nodes.extend( env.Install(targetDir, i) )
    return nodes

def PublicHeaders(env, targetDir, source):
    if targetDir:

        # install headers to build's include directory
        targetDir = os.path.join(env['INCLUDE_DIR'], targetDir)

    return InstallFiles(env, targetDir, source)

def PublicBinaries(env, source):
    return InstallFiles(env, env['BIN_DIR'], source)

def DistributeFiles(env, source, distPath=None, distFile=None, distGroup=None):
    global registry
    if SCons.Util.is_String(source):
        source = env.Split(source)
    elif not SCons.Util.is_List(source):
        source = [ source ]

    source = SourceGlobExpand(env, source)
    for i in source:
        if SCons.Util.is_String(i):
            i = env.File(i)
        if distPath is not None:
            distName = distPath + '/' + os.path.basename(str(i))
        elif distFile is not None:
            distName = distFile
        else:
            distName = None
        AddDistFile(i, distName, distGroup)

def CreateDist(env, targetFile, distGroups=None, distRootDir=None,
               distFormat='bz2', excludeExts=None, excludeDirs=None):
    global registry
    if distGroups is None:
        distGroups = ['default']
    elif not SCons.Util.is_List(distGroups):
        distGroups = [ distGroups ]
    source = []
    nameMap = {}
    for g in distGroups:
        registry.copyDistInfos(g, source, nameMap)
    return env.DistTar(targetFile, source,
                       DISTTAR_NAME_MAP=nameMap,
                       DISTTAR_DIR_NAME=distRootDir,
                       DISTTAR_FORMAT=distFormat,
                       DISTTAR_EXCLUDEEXTS=excludeExts,
                       DISTTAR_EXCLUDEDIRS=excludeDirs)

def GetRootDir(env):
    return env.Dir('#').abspath

def GetLibDir(env):
    return env['LIB_DIR']

def GetIncludeDir(env):
    return env['INCLUDE_DIR']


# _matches: portions from SCons-Wiki : BuildDirGlob
def _matches(file, includes, excludes=None, case_sensitive=True):
    """returns True if file matches the one pattern from the includes list
    and do not match any pattern from the excludes list.
    sensitive parameter controls whether case-sensitive comparison should be
    used

    includes and excludes should be iterable
    """
    if case_sensitive:
        matchFun = fnmatch.fnmatchcase
    else:
        matchFun = fnmatch.fnmatch
    
    match = False
    for pattern in includes:
        if matchFun(file, pattern):
            match = True
            break
    if match and not excludes is None:
        for pattern in excludes:
            if matchFun(file, pattern):
                match = False
                break
    return match


def matches(file, includes, excludes=None, case_sensitive=True):
    if SCons.Util.is_String(includes):
        includes = includes.split()

    if SCons.Util.is_String(excludes):
        excludes = excludes.split()

    return _matches(file, includes, excludes, case_sensitive)

# 
def FilterFiles(env, files, includes=None, excludes=None, match_function=None,
                case_sensitive=True,
                match_basename=True):
    """FilterFiles function filter files by specifying include and exclude
       patterns.

       match_function  if not None will be used instead of the default
                       glob pattern matching function.
                       Matching functions takes five arguments:
                       match_function(filename, includes, excludes, case_sensitive)
       case_sensitive  if true case-sensitive matching will be used
       match_basename  if true file will be matched after its basename,
                       otherwise after absolute path
    """
    if SCons.Util.is_String(files):
        files = env.Split(files)

    if SCons.Util.is_String(includes):
        includes = env.Split(includes)
    elif includes is None:
        includes = []

    if SCons.Util.is_String(excludes):
        excludes = env.Split(excludes)
    elif excludes is None:
        excludes = []

    if match_function is None:
        match_function = _matches

    result = []
    
    for i in files:

        # create a node from i
        
        if SCons.Util.is_String(i):
            try:
                n = env.Entry(i)
            except:
                # no Entry class available !
                if os.path.isdir(i):
                    n = env.Dir(i)
                else:
                    n = env.File(i)
        else:
            # we expect that when
            # i is not a string it is a node
            n = i

        # fn is a filename to match
        fn = n.abspath

        if match_basename:
            fn = os.path.basename(fn)
        
        if match_function(fn, includes, excludes, case_sensitive):
            result.append(n)
            
    return result

# FilterOut is a replacement for make's filter-out function.
# Currently stringsToFilter cannot contain patterns like
# supported by filter-out
def FilterOut(env, stringsToFilter, text, match_basename=True):
    s = env.Split(stringsToFilter)
    if SCons.Util.is_String(text):
        l = env.Split(text)
    else:
        l = text

    result = []
    for i in l:
        try:
            # is i a node ?
            fn = i.abspath

            if match_basename:
                fn = os.path.basename(fn)
                
                if fn not in s:
                    result.append(i)
        except:
            if str(i) not in s:
                result.append(i)
        
    return result

def AddPrefix(env, prefix, text):
    return map(lambda s: prefix+s, env.Split(text))

def PhonyTarget(env, alias, action):
    """Returns an alias to a command that performs the
    action.  This is implementated by a Command with a
    nonexistant file target.  This command will run on every
    build, and will never be considered 'up to date'. Acts
    like a 'phony' target in make."""
    from tempfile import mktemp
    from os.path import normpath
    phonyFile = normpath(mktemp(prefix='phony_%s_' % alias, dir='.'))
    return env.Alias(alias, env.Command(target=phonyFile,
                                        source=None, action=action))

class ConfigHeader:

    def __init__(self, filename='', text=''):
        self.filename = filename
        self.text = str(text)
        self.havedict = {}

    def getText(self):
        return self.text

    def getFilename(self):
        return self.filename

    def getHavedict(self):
        return self.havedict

    def addLine(self, line):
        self.text = self.text + line + '\n'

    def addConfigKey(self, key, have):
        '''
        Store result of a test in context.havedict and context.headerfilename.
        "key" is a "HAVE_abc" name.  It is turned into all CAPITALS and non-
        alphanumerics are replaced by an underscore.
        The value of "have" can be:
        1      - Feature is defined, add "#define key".
        0      - Feature is not defined, add "/* #undef key */".
                 Adding "undef" is what autoconf does.  Not useful for the
                 compiler, but it shows that the test was done.
        number - Feature is defined to this number "#define key have".
                 Does not work for 0 or 1, use a string then.
        string - Feature is defined to this string "#define key have".
                 Give "have" as is should appear in the header file,
                 include quotes when desired and escape special characters!
        '''
        key_up = key.upper()
        key_up = re.sub('[^A-Z0-9_]', '_', key_up)
        self.havedict[key_up] = have
        if have == 1:
            line = "#define %s\n" % key_up
        elif have == 0:
            line = "/* #undef %s */\n" % key_up
        elif type(have) == IntType:
            line = "#define %s %d\n" % (key_up, have)
        else:
            line = "#define %s %s\n" % (key_up, str(have))

        self.text = self.text + line

    def __str__(self):
        return "%s: %s" % (self.filename, self.text)

    def __repr__(self):
        return 'ConfigHeader(%s, %s)' % (repr(self.filename), repr(self.text))

def CreateConfigHeader(env, filename='', text=''):
    return ConfigHeader(filename, text)

def GetConfigHeader(env):
    return env['CONFIG_HEADER']

def AddConfigKeyEnv(env, key, have):
    config_h = GetConfigHeader(env)
    if config_h:
        config_h.addConfigKey(key, have)
        return True
    return False

# AddConfigKey is used with configuration contexts

def AddConfigKey(context, key, have):
    config_h = getattr(getattr(context, 'sconf'), 'sb_config_h', None)
    if config_h:
        config_h.addConfigKey(key, have)
        return True
    return False

def GenerateConfigHeader(env, target, config_header=None):
    headerSrcInfo = 'config_header argument'
    if config_header == None:
        config_header = env.get('CONFIG_HEADER', None)
        headerSrcInfo = "env['CONFIG_HEADER']"
    if not isinstance(config_header, ConfigHeader):
        raise SCons.Errors.UserError, \
              "%s must be of type ConfigHeader but is: %s" % \
              (headerSrcInfo, str(type(config_header)))
    return env.ConfigHBuilder(target = target,
                              source = env.Value(config_header.getText()))

# Options tools
# Portions from SCons Options/__init__.py

def LoadOptions(files, keys=[]):
    """Loads configuration options from SCons options file in Python format
       returns dictionary
    files - List of option configuration files to load
            If a single string is passed it is
            automatically placed in a file list
    keys  - List of keys which should be returned in the dictionary
            If a single string is passed it is
            automatically placed in a keys list
    """
    if SCons.Util.is_String(files):
        files = [ files ]
    if SCons.Util.is_String(keys):
        keys = [ keys ]
    values = {}
    for filename in files:
        if os.path.exists(filename):
            execfile(filename, values)
    r = {}
    for key in keys:
        try:
            r[key] = values[key]
        except KeyError:
            pass
    return r

def SaveOptions(filename, options, keys=None):
    """Saves configuration options as Python script
    filename - Name of the file to save into
    options  - Dictionary with the values to save
    keys     - List of keys which should be saved to the file
               If keys argument is empty all options will be saved
    """
    # Create the file and write out the header
    try:
        fh = open(filename, 'w')

        try:
            # Make an assignment in the file for each option within the
            # options dictionary that was assigned a value other than
            # the default.
            for key, value in options.items():
                if keys and key not in keys:
                    continue
                try:
                    eval(repr(value))
                except KeyboardInterrupt:
                    raise
                except:
                    # Convert stuff that has a repr() that
                    # cannot be evaluated into a string
                    value = SCons.Util.to_String(value)

                fh.write('%s = %s\n' % (key, repr(value)))
        finally:
            fh.close()

    except IOError, x:
        raise SCons.Errors.UserError, 'Error writing options to file: %s\n%s' % (filename, x)

# Path Tools

def SplitPath(path, sep = os.pathsep):
    if not SCons.Util.is_List(path) and not SCons.Util.is_Tuple(path):
        return string.split(path, sep)
    else:
        return path

AppendPath = SCons.Util.AppendPath

def RemovePath(oldpath, removepath, sep = os.pathsep):
    """This removes path elements from the given old path.
    This can also handle the case where the given old
    path variable is a list instead of a string, in which case a list
    will be returned instead of a string.

    Example:
      Old Path: "/foo/bar:/foo"
      Remove Path: "/foo/bar"
      Result:   "/foo"
    """

    orig = oldpath
    is_list = 1
    paths = orig
    if not SCons.Util.is_List(orig) and not SCons.Util.is_Tuple(orig):
        paths = string.split(paths, sep)
        is_list = 0

    if SCons.Util.is_List(removepath) or SCons.Util.is_Tuple(removepath):
        removepaths = removepath
    else:
        removepaths = string.split(removepath, sep)
        
    paths = [i for i in paths if i not in removepaths]

    if is_list:
        return paths
    else:
        return string.join(paths, sep)


# Register new global environment functions

AddMethod(Environment, SetupEnvironment)
AddMethod(Environment, SaveVars)
AddMethod(Environment, RestoreVars)
AddMethod(Environment, DeclarePackage)
AddMethod(Environment, GetPackage)
AddMethod(Environment, AddPackageTrigger)
AddMethod(Environment, RequirePackage)
