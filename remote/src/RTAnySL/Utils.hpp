#ifndef RTANYSL_UTILS_HPP_INCLUDED
#define RTANYSL_UTILS_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <cstdlib>
#include <iostream>
#include <jitRT/llvmWrapper.h>
#include <cassert>

namespace RTAnySL
{

#define JITRT_SHADER_BUILD_DIR_ENVVAR "JITRT_SHADER_BUILD_DIR"
#define JITRT_BITCODE_PATH_ENVVAR "JITRT_BITCODE_PATH"

#if !defined(NDEBUG) || defined(RTANYSL_DO_DEBUG)
#define RTANYSL_DEBUG_MODE
#endif

#ifdef RTANYSL_DEBUG_MODE
#define RTANYSL_IFDEBUG(stmt) stmt
#else
#define RTANYSL_IFDEBUG(stmt)
#endif

#define RTANYSL_MSG(out, msg) \
    do { (out)<<"RTAnySL: "<<msg<<std::endl; } while (false)

#define RTANYSL_LOG(msg) RTANYSL_MSG(std::cout, msg)

#define RTANYSL_WARNING(msg) RTANYSL_MSG(std::cerr, "Warning: "<<msg)

#define RTANYSL_ERROR(msg) RTANYSL_MSG(std::cerr, "Error: "<<msg)

#define RTANYSL_DEBUG(msg) \
    RTANYSL_IFDEBUG( RTANYSL_MSG(std::cout, "Debug: "<<msg) )

#define RTANYSL_ASSERT(cond, msg) assert((cond) && (msg))

/// Data structure for defining API functions
struct APIFunc
{
    const char *name;
    void * func;
};

/// Macros for defining API function table
#define RTANYSL_BEGIN_API_TABLE(varname)         \
    static const RTAnySL::APIFunc varname [] = {

#define RTANYSL_API_FUNC(name, func)             \
        {name, (void*) func },

#define RTANYSL_END_API_TABLE()                  \
        {0, 0}                                   \
    };

class RTANYSL_API Utils
{
public:

    class RTANYSL_API PathFinder
    {
    public:

        struct FromEnvVarTag { };

        PathFinder();

        PathFinder(const char *path);

        PathFinder(const char *envVarName, FromEnvVarTag);

        void clearSearchPaths()
        {
            searchPaths.clear();
        }

        void appendSearchPath(const std::string &path)
        {
            searchPaths.push_back(path);
        }

        void setSearchPathsFromEnvVar(const char *envVarName);

        std::string findPath(const std::string &fileName) const;

    private:
        std::vector<std::string> searchPaths;
    };

    /// Returns path finder for BC files, which searches in paths stored in
    /// JITRT_BITCODE_PATH_ENVVAR environment variable.
    static const PathFinder & getBitcodePathFinder();

    /// Returns true if apiFuncName was found in shaderModule and
    /// successfully linked with apiFunc
    static bool linkAPIFunc(llvm::Module * shaderModule,
                            const std::string &apiFuncName,
                            void * apiFunc);

    /// Link all API functions from the table.
    /// Table is declared with RTANYSL_BEGIN_API_TABLE, RTANYSL_API_FUNC, and
    /// RTANYSL_END_API_TABLE.
    static void linkAPIFuncTable(llvm::Module * shaderModule,
                                 const APIFunc * apiFuncTable);

    static std::string getBuildDir();

    static bool removeFile(const std::string &fileName);

    static std::string makeTempFileName(const std::string &baseName);

    static bool isFileExists(const std::string &path)
    {
        return isFileExists(path.c_str());
    }

    static bool isFileExists(const char *path);
};

} // namespace RTAnySL

#endif
