/* 
 *  PathUtils.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2010  Saarland University
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
 *  Created on: 2008-11-02 21:38:37 +0100
 *  Author(s): Iliyan Georgiev, Lukas Marsalek
 */
 
 #ifndef RTFACT__PATH_UTILS_HPP
#define RTFACT__PATH_UTILS_HPP

#include <RTfact/Config/Common.hpp>

#include <string>
#include <locale>
#include <cctype>
#include <boost/algorithm/string/trim.hpp>

#include <RTfact/Utils/ReferenceCounted.hpp>
#include <RTfact/Utils/IO/Configuration.hpp>

#define RTFACT__PATH_SEPARATOR_CHAR_UNIX '/'
#define RTFACT__PATH_SEPARATOR_CHAR_WIN '\\'

#define RTFACT__PATH_SEPARATOR_STR_UNIX "/"
#define RTFACT__PATH_SEPARATOR_STR_WIN "\\"

#ifdef RTFACT_OS_WIN
#   define RTFACT__PATH_SEPARATOR_CHAR RTFACT__PATH_SEPARATOR_CHAR_WIN
#   define RTFACT__PATH_SEPARATOR_STR  RTFACT__PATH_SEPARATOR_STR_WIN
#else
#   define RTFACT__PATH_SEPARATOR_CHAR RTFACT__PATH_SEPARATOR_CHAR_UNIX
#   define RTFACT__PATH_SEPARATOR_STR  RTFACT__PATH_SEPARATOR_STR_UNIX
#endif

RTFACT_NAMESPACE_BEGIN

namespace IO {

    typedef std::list<std::string> PathList;

namespace Detail
{
    RTFACT_INLINE void replaceAll(
        std::string& aWhere, 
        const std::string& aWhat, 
        const std::string& aWith) 
    {
        std::string::size_type i = 0;
        while (i < aWhere.size()) {
            i = aWhere.find(aWhat, i);
            if (i == std::string::npos) 
                break;
            aWhere.replace(i, aWhat.size(), aWith);
            i+=aWith.size();
        }
    }

    RTFACT_INLINE bool isAbsoluteWinPath(
        const std::string &path)
    {
        // "[A-Za-z]:\"
        if (path.size() > 2)
        {
            return (isascii(path[0]) && isalpha(path[0]) &&
                    (path[1] == ':') &&
                    (path[2] == RTFACT__PATH_SEPARATOR_CHAR_WIN));
        }
        return false;
    }

    RTFACT_INLINE bool isAbsoluteUnixPath(
        const std::string &path)
    {
        return path.size() > 0 && path[0] == RTFACT__PATH_SEPARATOR_CHAR_UNIX;
    }

    RTFACT_INLINE const std::string convertUnixToNativePath(
        const std::string& aPath)
    {
        #if defined(RTFACT_OS_WIN)
            // unix -> windows
            if (aPath.size() > 0) {
                std::string winPath = aPath;

                // drop first '/' if we have something like "/[A-Za-z]:",
                // otherwise we may have UNC path
                if (winPath.size() > 2 && winPath[0] == RTFACT__PATH_SEPARATOR_CHAR_UNIX &&
                    isascii(winPath[1]) && isalpha(winPath[1]) &&
                    winPath[2] == RTFACT__PATH_SEPARATOR_CHAR_UNIX)
				{
                    winPath.erase(0, 1);
					winPath.insert(1, 1, ':');
				}	

                replaceAll(winPath, RTFACT__PATH_SEPARATOR_STR_UNIX, RTFACT__PATH_SEPARATOR_STR_WIN);
                return winPath;
            }
            return aPath;
        #else
            // posix and all other
            return aPath;
        #endif
    }

    RTFACT_INLINE const std::string convertNativeToUnixPath(
        const std::string& aPath)
    {
        #if defined(RTFACT_OS_WIN)
            // windows -> unix
            if (aPath.size() > 0) {
                std::string unixPath = aPath;

                replaceAll(unixPath, RTFACT__PATH_SEPARATOR_STR_WIN, RTFACT__PATH_SEPARATOR_STR_UNIX);

                // if we have "[A-Za-z]:", prepend '/' e.g. c:/aa/bb -> /c/aa/bb as the msysgit does
                if (unixPath.size() > 1 &&
                    isascii(unixPath[0]) && isalpha(unixPath[0]) &&
					unixPath[1] == ':') 
				{
					unixPath.erase(1,1);
					unixPath.insert(0, 1, RTFACT__PATH_SEPARATOR_CHAR_UNIX);
                }

                return unixPath;
            }
            return aPath;
        #else
            // posix and all other
            return aPath;
        #endif
    }
} //namespace Detail

RTFACT_INLINE bool isAbsolutePath(
    const std::string &path)
{
#ifdef RTFACT_OS_WIN
    return Detail::isAbsoluteWinPath(path);
#else
    // posix and the the rest
    return Detail::isAbsoluteUnixPath(path);
#endif
}

RTFACT_INLINE const std::string getParentDirectory(
    const std::string& aPath) 
{
    std::string trimmedPath = aPath;
    boost::algorithm::trim(trimmedPath);

    std::string unixPath = Detail::convertNativeToUnixPath(trimmedPath);

    // (1) Strip trailing slashes
    size_t pos = unixPath.find_last_not_of('/');
    // (2) If string consists entirely of slash characters, string shall be
    //     set to a single slash character.
    if (pos == std::string::npos && unixPath.size() > 0) {
        return "/";
    }
    // (3) If there are any trailing slash characters in string, they
    //     shall be removed.
    std::string dir(unixPath, 0, pos+1);

    // (4) If there are no slash characters remaining in name, name
    //     shall be set to a single period character.  In this case, skip
    //     steps (5) through (7).
    pos = dir.find_last_of('/');
    if (pos == std::string::npos) {
        return ".";
    }
    // (5) If there are any trailing nonslash characters in string,
    //     they shall be removed. 
    dir.resize(pos+1);
    // (6) If there are any trailing slash characters in string, they
    //     shall be removed.
    pos = dir.find_last_not_of('/');
    if (pos == std::string::npos) {
        // (8) If the remaining string is empty, string shall be set to a single
        //      slash character
        return "/";
    }
    dir.resize(pos+1);

    return Detail::convertUnixToNativePath(dir);
}

RTFACT_INLINE const std::string appendPath(
    const std::string &aPath1,
    const std::string &aPath2)
{
    std::string path1 = aPath1;
    std::string path2 = aPath2;

    boost::algorithm::trim(path1);
    boost::algorithm::trim(path2);

    size_t size1 = path1.size();
    size_t size2 = path2.size();
   
    std::string result(path1);
    
    if (size1 > 0 &&
        size2 > 0 &&
        path1[size1-1] != RTFACT__PATH_SEPARATOR_CHAR &&
        path2[0] != RTFACT__PATH_SEPARATOR_CHAR)
    {
        result.append(RTFACT__PATH_SEPARATOR_STR);
    }

    result.append(path2);

    return result;
}

RTFACT_INLINE const std::string toLower(
    const std::string& aString)
{
    return boost::to_lower_copy(aString);
}

/**
 * Returns true, if the path is absolute(e.g. starts with '/' on Unix or with 'X:' on Windows
 */
RTFACT_INLINE bool isPathAbsolute(std::string aPath)
{
	std::string unixPath = Detail::convertNativeToUnixPath(aPath);
	return ((unixPath.size() > 0) && (unixPath[0] == RTFACT__PATH_SEPARATOR_CHAR_UNIX));
}

} // namespace IO

RTFACT_NAMESPACE_END

#undef RTFACT__PATH_SEPARATOR_CHAR_UNIX
#undef RTFACT__PATH_SEPARATOR_CHAR_WIN
#undef RTFACT__PATH_SEPARATOR_STR_UNIX
#undef RTFACT__PATH_SEPARATOR_STR_WIN
#undef RTFACT__PATH_SEPARATOR_CHAR
#undef RTFACT__PATH_SEPARATOR_STR

#endif // RTFACT__PATH_UTILS_HPP
