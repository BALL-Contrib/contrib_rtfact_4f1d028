/* 
 *  cpuinfo.cpp
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
 *  Created on: 2009-02-22 00:29:53 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #include <RTfact/Utils/Containers/Vector.hpp>
#include <RTfact/Utils/System/CPUInfo.hpp>
#include <RTfact/Utils/IO/PathUtils.hpp>

#include <string>

#ifdef RTFACT_OS_WIN
#   define RTFACT__DIR_SEP '\\'
#else
#   define RTFACT__DIR_SEP '/'
#endif

using namespace RTfact;
using namespace std;

void printHelp(
    const string aProgramName)
{
    RTFACT_LOG_LINE("");
    RTFACT_LOG_LINE_F("Usage: %1% [arguments]", aProgramName);
    RTFACT_LOG_LINE("");
    RTFACT_LOG_LINE("Options:");
    RTFACT_LOG_LINE("  -h, --help                    prints this message");
    RTFACT_LOG_LINE("  -a, --all                     prints all CPU capabilities that are checked for");
    RTFACT_LOG_LINE("  -v, --available               prints all capabilities available on the CPU");
    RTFACT_LOG_LINE("  -c, --check [capability1 ..]  checks for specified capabilities; prints 1 if");
    RTFACT_LOG_LINE("                                all are available and returns 0, otherwise");
    RTFACT_LOG_LINE("                                prints 0 and returns 1");
    RTFACT_LOG_LINE("Example:");
    RTFACT_LOG_LINE_F("  %1% -c sse2 sse4_1", aProgramName);
    RTFACT_LOG_LINE("                                prints 1 and returns 0 if both SSE 2 and SSE 4.1");
    RTFACT_LOG_LINE("                                are available on the current CPU");
    RTFACT_LOG_LINE("");

    #if defined(RTFACT_DEBUG) && defined(RTFACT_OS_WIN)

        RTFACT_BREAK_CONSOLE();

    #endif
}

int main(
    int argc,
    char** argv)
{
    CPUInfo cpuInfo;

    string programName = argv[0];

    size_t sepIndex = programName.find_last_of(RTFACT__DIR_SEP);

    if(sepIndex != string::npos)
    {
        ++sepIndex;
    }

    programName = programName.substr(sepIndex, programName.size() - sepIndex);

    Vector<string> args;
    args.reserve(argc - 1);

    for(int i = 1; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }

    const unsigned int argCount = args.size();

    if(argCount == 0 || (argCount == 1 && (args[0] == "-h" || args[0] == "--help")))
    {
        printHelp(programName);

        return 0;
    }
    else if(argCount == 1)
    {
        if(args[0] == "-a" || args[0] == "--all")
        {
            RTFACT_LOG_LINE(cpuInfo.getAll());
        }
        else if(args[0] == "-v" || args[0] == "--available")
        {
            RTFACT_LOG_LINE(cpuInfo.getAllAvailable());
        }
        else
        {
            printHelp(programName);

            return 1;
        }
    }
    else
    {
        if(args[0] != "-c" && args[0] != "--check")
        {
            printHelp(programName);

            return 1;
        }
        else if(argCount == 2)
        {
            int res = cpuInfo.get(args[1]);

            RTFACT_LOG_LINE(res);

            return res == 0;
        }
        else
        {
            for(size_t i = 1; i < args.size(); ++i)
            {
                if(!cpuInfo.get(args[i]))
                {
                    RTFACT_LOG_LINE("0");

                    return 1;
                }
            }

            RTFACT_LOG_LINE("1");

            return 0;
        }
    }

    return 0;
}
