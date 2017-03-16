/*
 *  SystemUtils.hpp
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
 *  Author(s): Iliyan Georgiev, Dmitri Rubinstein, Rossen Dimov
 */

 #ifndef RTFACT__SYSTEMUTILS_HPP
#define RTFACT__SYSTEMUTILS_HPP

// Note: this file is included by RTfact/Config/Common.hpp

#ifndef RTFACT_OS_MACOS
#   include <malloc.h>
#endif

#ifdef RTFACT_OS_WIN
#   ifndef NOMINMAX
#     define NOMINMAX 1
#   endif
#   include <windows.h> // for performance measurements
#else
#   include <unistd.h>
#   include <cerrno>
#endif

RTFACT_NAMESPACE_BEGIN

RTFACT_FORCE_INLINE uint64 getTimestamp()
{
    #if defined(RTFACT_COMPILER_INTEL)

        return _rdtsc();

    #elif defined(RTFACT_COMPILER_MSVC)

        return __rdtsc();

    #elif defined(RTFACT_COMPILER_GCC)

        #if defined(__i386__)

            unsigned long long int x;
            __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
            return x;

        #elif defined(__x86_64__)

            unsigned hi, lo;
            __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
            return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );

        #endif

    #else

        #error Unsupported compiler: provide implementation of getTimestamp()

    #endif
}

RTFACT_FORCE_INLINE uint64 getPerformanceCounterResolution()
{
    static uint64 ret = 0;

    if(ret == 0)
    {
        #if defined(RTFACT_OS_WIN)

            ::SetThreadAffinityMask(::GetCurrentThread(), 1);

            //::QueryPerformanceFrequency((LARGE_INTEGER*)&ret);

            //uint64 t1;
            //QueryPerformanceCounter((LARGE_INTEGER*)&t1);

            const uint64 t1 = getTimestamp();

            Sleep(1000);

            const uint64 t2 = getTimestamp();

            //uint64 t2;
            //QueryPerformanceCounter((LARGE_INTEGER*)&t2);

            ret = 2600000000;
            //ret = t2 - t1;
            //std::cout << "<- " <<  ret << std::endl;

        #else

            const uint64 t1 = getTimestamp();

            usleep(1000000L);

            const uint64 t2 = getTimestamp();

            ret = t2 - t1;

        #endif

        RTFACT_LOG_LINE_F("Measured CPU frequency is %1%GHz", (ret / 1000000000.0));
    }

    return ret;
}

RTFACT_FORCE_INLINE void* alignedMalloc(
    const size_t bytes,
    const size_t alignment)
{
    #if defined(RTFACT_OS_WIN)

        #ifdef RTFACT_COMPILER_GCC

            return __mingw_aligned_malloc(bytes, alignment);

        #else

            return _aligned_malloc(bytes, alignment);

        #endif

    #elif defined(RTFACT_OS_MACOS)

        // On Mac OS X malloc always return aligned memory
        return malloc(bytes);

    #else

        void *res;

        int err = posix_memalign(&res, alignment, bytes);

        #ifdef RTFACT_DEBUG

            if(err != 0)
            {
                RTFACT_LOG_LINE_F("posix_memalign(alignment=%1%, size=%2%) == ",
                       alignment % bytes);

                switch(err)
                {
                    case EINVAL:
                        RTFACT_LOG_LINE("EINVAL");
                        RTFACT_LOG_LINE_F("sizeof(void*) == %1%", sizeof(void*));
                        RTFACT_LOG_LINE_F("alignment / sizeof(*void) == %1%",
                               (float(alignment) / sizeof(void*)));
                        break;
                    case ENOMEM:
                      RTFACT_LOG_LINE("ENOMEM"); break;
                    default:
                      RTFACT_LOG_LINE_F("%1%", err); break;
                }
            }
        #endif

        return (err == 0) ? res : 0;

    #endif
}

RTFACT_FORCE_INLINE void alignedFree(
    void *ptr)
{
    #if defined(RTFACT_OS_WIN)

        #ifdef RTFACT_COMPILER_GCC

            __mingw_aligned_free(ptr);

        #else

            _aligned_free(ptr);

        #endif

    #else

        free(ptr);

    #endif
}

RTFACT_NAMESPACE_END

#endif // RTFACT__SYSTEMUTILS_HPP
