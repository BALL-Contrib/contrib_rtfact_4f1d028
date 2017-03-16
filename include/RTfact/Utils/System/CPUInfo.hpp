/* 
 *  CPUInfo.hpp
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
 *  Created on: 2009-02-20 14:23:05 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__CPUINFO_HPP
#define RTFACT__CPUINFO_HPP

#include <RTfact/Config/Common.hpp>

#if defined(RTFACT_COMPILER_MSVC) ||                                           \
   (defined(RTFACT_COMPILER_INTEL) && defined(RTFACT_OS_WIN))
#
#   include <intrin.h>
#
#   define RTFACT__CPUID(aInfo, aInfoType) __cpuid(aInfo, aInfoType)
#
#elif defined(RTFACT_OS_POSIX) || defined(RTFACT_OS_MACOS)
#
#   define RTFACT__CPUID(aInfo, aInfoType)                                     \
        __asm__ __volatile__ ("cpuid":                                         \
            "=a" (aInfo[0]), "=b" (aInfo[1]), "=c" (aInfo[2]),                 \
            "=d" (aInfo[3]) : "a" (aInfoType));
#
#endif

#define RTFACT__INFO_TYPE_IDENTIFICATION    0
#define RTFACT__INFO_TYPE_CAPABILITIES      1
#define RTFACT__INFO_TYPE_EXT               0x80000000
#define RTFACT__INFO_TYPE_EXT1              0x80000001
#define RTFACT__INFO_TYPE_EXT6              0x80000006

#include <string.h>
#include <string>
#include <locale>
#include <algorithm>
#include <sstream>
#include <map>

#include <RTfact/Utils/IO/PathUtils.hpp>

#define RTFACT__EXTRACT(aData, aMask)  ((aData) & (aMask))
#define RTFACT__CHECK_BIT(aData, aBit) (((aData) & (1 << (aBit))) != 0)

RTFACT_NAMESPACE_BEGIN

class CPUInfo
{
protected:

    typedef std::map<std::string, int> t_CapabilityMap;

    t_CapabilityMap mCapabilities;

    void reset()
    {
        mCapabilities.clear();
    }

    void updateProtected()
    {
        int info[4];

        ////////////////////////////////////////////////////////////////////
        // Get CPU brand
        ////////////////////////////////////////////////////////////////////

        RTFACT__CPUID(info, RTFACT__INFO_TYPE_IDENTIFICATION);

        char brandChars[0x20];

        memset(brandChars, 0, sizeof(brandChars));

        *((int*) brandChars)       = info[1];
        *((int*) (brandChars + 4)) = info[3];
        *((int*) (brandChars + 8)) = info[2];

        mCapabilities["intel"] = 0;
        mCapabilities["amd"] = 0;

        if(strcmp(brandChars, "GenuineIntel") == 0)
        {
            mCapabilities["intel"] = 1;
        }

        if(strcmp(brandChars, "AuthenticAMD") == 0)
        {
            mCapabilities["amd"] = 1;
        }

        ////////////////////////////////////////////////////////////////////
        // Get basic capabilities
        ////////////////////////////////////////////////////////////////////

        RTFACT__CPUID(info, RTFACT__INFO_TYPE_CAPABILITIES);

        mCapabilities["mmx"]    = RTFACT__CHECK_BIT(info[3], 23);
        mCapabilities["sse"]    = RTFACT__CHECK_BIT(info[3], 25);
        mCapabilities["sse2"]   = RTFACT__CHECK_BIT(info[3], 26);
        mCapabilities["sse3"]   = RTFACT__CHECK_BIT(info[2],  0);
        mCapabilities["ssse3"]  = RTFACT__CHECK_BIT(info[2],  9);
        mCapabilities["sse4_1"] = RTFACT__CHECK_BIT(info[2], 19);
        mCapabilities["sse4_2"] = RTFACT__CHECK_BIT(info[2], 20);
        mCapabilities["popcnt"] = RTFACT__CHECK_BIT(info[2], 23);
        mCapabilities["x87"]    = RTFACT__CHECK_BIT(info[3],  0);
        mCapabilities["cores"]  = RTFACT__EXTRACT(info[1] >> 16, 0xF);

        ////////////////////////////////////////////////////////////////////
        // Get extended capabilities
        ////////////////////////////////////////////////////////////////////

        RTFACT__CPUID(info, RTFACT__INFO_TYPE_EXT);

        const int maxExtInfo = info[0];

        ////////////////////////////////////////////////////////////////////
        // Get extended capabilities 1
        ////////////////////////////////////////////////////////////////////

        if(maxExtInfo >= RTFACT__INFO_TYPE_EXT1)
        {
            RTFACT__CPUID(info, RTFACT__INFO_TYPE_EXT1);

            mCapabilities["3dnow"]      = 0;
            mCapabilities["3dnowext"]   = 0;
            mCapabilities["mmxext"]     = 0;
            mCapabilities["sse4a"]      = 0;
            mCapabilities["amd_rdtscp"] = 0;

            if(get("amd"))
            {
                mCapabilities["3dnow"]      = RTFACT__CHECK_BIT(info[3], 31);
                mCapabilities["3dnowext"]   = RTFACT__CHECK_BIT(info[3], 30);
                mCapabilities["mmxext"]     = RTFACT__CHECK_BIT(info[3], 22);
                mCapabilities["sse4a"]      = RTFACT__CHECK_BIT(info[2],  6);
                mCapabilities["amd_rdtscp"] = RTFACT__CHECK_BIT(info[3], 27);
            }

            mCapabilities["x64"] = RTFACT__CHECK_BIT(info[3], 29);
        }

        ////////////////////////////////////////////////////////////////////
        // Get extended capabilities 6
        ////////////////////////////////////////////////////////////////////

        if(maxExtInfo >= RTFACT__INFO_TYPE_EXT6)
        {
            RTFACT__CPUID(info, RTFACT__INFO_TYPE_EXT6);

            mCapabilities["cache_line"] = RTFACT__EXTRACT(info[2], 0xFF);
            mCapabilities["cache"]      = RTFACT__EXTRACT(info[2] >> 16, 0xFFFF);
            mCapabilities["cache_associativity"] = RTFACT__EXTRACT(info[2] >> 12, 0xF);
        }
    }

public:

    CPUInfo(
        const bool aReadInfo = true)
    {
        reset();

        if(aReadInfo)
        {
            updateProtected();
        }
    }

    void update()
    {
        reset();
        updateProtected();
    }

    int get(
        const std::string& aCapability) const
    {
        std::string capability = IO::toLower(aCapability);

        t_CapabilityMap::const_iterator it = mCapabilities.find(capability);

        if(it == mCapabilities.end())
        {
            return 0;
        }
        else
        {
            return it->second;
        }
    }

    std::string getAll() const
    {
        std::ostringstream outStream;

        for(t_CapabilityMap::const_iterator it = mCapabilities.begin();
            it != mCapabilities.end(); ++it)
        {
            outStream << it-> first << ' ';
        }

        return outStream.str();
    }

    std::string getAllAvailable() const
    {
        std::ostringstream outStream;

        for(t_CapabilityMap::const_iterator it = mCapabilities.begin();
            it != mCapabilities.end(); ++it)
        {
            if(it->second)
            {
                outStream << it-> first << ' ';
            }
        }

        return outStream.str();
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__CPUINFO_HPP
