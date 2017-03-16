#include "Precompiled.hpp"

#define RTANYSL_LIB
#include <RTAnySL/Config/Common.hpp>

// TODO : replace following header with the internal implementation
//        in order to remove dependency to RTfact
#include <RTfact/Utils/IO/PathUtils.hpp>

#include <RTAnySL/Utils.hpp>
#include <fstream>

#ifdef RTANYSL_OS_WIN
#include <windows.h>
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace RTAnySL
{

// Utils

// Utils::PathFinder

Utils::PathFinder::PathFinder()
{
}

Utils::PathFinder::PathFinder(const char *path)
{
    if (path)
        searchPaths.push_back(path);
}

Utils::PathFinder::PathFinder(const char *envVarName,
                              Utils::PathFinder::FromEnvVarTag)
{
    setSearchPathsFromEnvVar(envVarName);
}

void Utils::PathFinder::setSearchPathsFromEnvVar(const char *envVarName)
{
    const char * envpath = getenv("JITRT_BITCODE_PATH");
    if (envpath)
    {
        // split directories
#ifdef RTFACT_OS_WIN
        const char *const searchPathSep = ";";
#else
        const char *const searchPathSep = ":";
#endif
        boost::algorithm::split(searchPaths, envpath,
                                boost::algorithm::is_any_of(
                                    searchPathSep));
    }
    else
    {
        // search in current working directory by default
        searchPaths.push_back(".");
    }
}

std::string Utils::PathFinder::findPath(
    const std::string &fileName) const
{
    if (Utils::isFileExists(fileName))
        return fileName;

    // don't search in searchPaths list if fileName is absolute
    if (!RTfact::IO::isAbsolutePath(fileName))
    {
        typedef std::vector<std::string>::const_iterator SVIter;
        for (SVIter it = searchPaths.begin();
             it != searchPaths.end(); ++it)
        {
            std::string fn = RTfact::IO::appendPath(*it, fileName);
            if (Utils::isFileExists(fn))
                return fn;
        }
    }

    RTANYSL_ERROR("Could not find "<<fileName);

    return "";
}

// Utils functions

const Utils::PathFinder & Utils::getBitcodePathFinder()
{
    static PathFinder _pathFinder(
        JITRT_BITCODE_PATH_ENVVAR, PathFinder::FromEnvVarTag());

    return _pathFinder;
}

bool Utils::linkAPIFunc(llvm::Module * shaderModule,
                        const std::string &apiFuncName,
                        void * apiFunc)
{
    RTANYSL_ASSERT(apiFunc != 0, "null is not a valid api function");

    llvm::Function * apiFuncDecl =
        jitRT::getFunction(apiFuncName, shaderModule);

    if (apiFuncDecl)
    {
		//jitRT::setFunctionDoesNotAlias(apiFuncDecl);
		//jitRT::setAllFunctionCallsDoNotAccessMemory(apiFuncDecl); //bad :P
        jitRT::replaceAllUsesWith(
            apiFuncDecl,
            jitRT::createFunctionPointer(apiFuncDecl, apiFunc));

        return true;
    }
    return false;
}

void Utils::linkAPIFuncTable(llvm::Module * shaderModule,
                             const APIFunc * apiFuncTable)
{
    RTANYSL_ASSERT(apiFuncTable != 0, "apiFuncTable is NULL");

    while (apiFuncTable->name != 0 && apiFuncTable->func != 0)
    {
        if (!RTAnySL::Utils::linkAPIFunc(shaderModule,
                                         apiFuncTable->name,
                                         apiFuncTable->func))
        {
            RTANYSL_LOG("API function '"<<apiFuncTable->name<<"' is not used");
        }
        ++apiFuncTable;
    }
}


std::string Utils::getBuildDir()
{
    const char *buildDir = ::getenv(JITRT_SHADER_BUILD_DIR_ENVVAR);
    // TODO : add support for Windows
    if (buildDir == 0)
        buildDir = ".";
    return buildDir;
}

bool Utils::removeFile(const std::string &fileName)
{
#ifdef RTFACT_OS_WIN
    return ::_unlink(fileName.c_str()) == 0;
#else
    return ::unlink(fileName.c_str()) == 0;
#endif
}

std::string Utils::makeTempFileName(const std::string &baseName)
{
    const char mktempTemplate[] = "_XXXXXX";
    const int templateSize = sizeof(mktempTemplate)/sizeof(char);

    char *tmp = new char[baseName.length()+templateSize];
    ::strcpy(tmp, baseName.c_str());
    ::strcpy(tmp+baseName.length(), mktempTemplate);

#ifdef RTFACT_OS_WIN
    ::_mktemp(tmp);
#else
    ::mktemp(tmp);
#endif
    std::string tempFile(tmp);

    delete [] tmp;

    return tempFile;
}

bool Utils::isFileExists(const char *path)
{
    FILE *fd = fopen(path, "r");
    if (fd)
    {
        fclose(fd);
        return true;
    }
    return false;
}

} // namespace RTAnySL
