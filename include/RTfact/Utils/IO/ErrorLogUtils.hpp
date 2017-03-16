/* 
 *  ErrorLogUtils.hpp
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
 *  Created on: 2008-11-02 21:38:37 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__ERRORLOGUTILS_HPP
#define RTFACT__ERRORLOGUTILS_HPP

// Note: this file is included by RTfact/Config/Common.hpp

#include <iostream>
#include <boost/format.hpp>

#if defined(RTFACT_DEBUG) || defined(RTFACT_FULL_LOGGING)
#   define RTFACT__LOG_FULL
#endif

RTFACT_NAMESPACE_BEGIN

#pragma region Message logging utilities

namespace Detail {

template<
    class tMessage>
RTFACT_INLINE void LogMessage(
    const char* aFile,
    const uint aLine,
    tMessage aMessage,
    const bool aEndLine,
    const bool aFullLog)
{
    #ifndef RTFACT_NO_STD_OUTPUT

        if(aFullLog)
        {
            std::cout << aFile << " (" << aLine << "): ";
        }

        std::cout << aMessage;

        if(aEndLine)
        {
            std::cout << std::endl;
        }

        std::cout << std::flush;

    #endif
}

} // namespace Detail

#define RTFACT_BREAK_CONSOLE()                                                 \
    std::cin.get()

#define RTFACT_LOG(aMessage)                                                   \
    ::RTfact::Detail::LogMessage(__FILE__, __LINE__, aMessage, false, false)

#define RTFACT_FULL_LOG(aMessage)                                              \
    ::RTfact::Detail::LogMessage(__FILE__, __LINE__, aMessage, false, true)

#define RTFACT_LOG_LINE(aMessage)                                              \
    ::RTfact::Detail::LogMessage(__FILE__, __LINE__, aMessage, true, false)

#define RTFACT_FULL_LOG_LINE(aMessage)                                         \
    ::RTfact::Detail::LogMessage(__FILE__, __LINE__, aMessage, true, true)

#define RTFACT_LOG_F(aFormat, aParams)                                         \
    RTFACT_LOG((boost::format(aFormat) % aParams).str())

#define RTFACT_FULL_LOG_F(aFormat, aParams)                                    \
    RTFACT_FULL_LOG((boost::format(aFormat) % aParams).str())

#define RTFACT_LOG_LINE_F(aFormat, aParams)                                    \
    RTFACT_LOG_LINE((boost::format(aFormat) % aParams).str())

#define RTFACT_FULL_LOG_LINE_F(aFormat, aParams)                               \
    RTFACT_FULL_LOG_LINE((boost::format(aFormat) % aParams).str())

#pragma endregion // Message logging utilities

#pragma region Warning logging macros

#ifdef RTFACT__LOG_FULL

#define RTFACT_LOG_WARNING(aMessage)                                           \
    RTFACT_FULL_LOG_LINE_F("Warning: %1%", (aMessage))

#define RTFACT_LOG_WARNING_F(aFormat, aParams)                                 \
    RTFACT_FULL_LOG_LINE_F("Warning: %1%", (boost::format(aFormat) % aParams).str())

#else

#define RTFACT_LOG_WARNING(aMessage)                                           \
    RTFACT_LOG_LINE_F("Warning: %1%", (aMessage))

#define RTFACT_LOG_WARNING_F(aFormat, aParams)                                 \
    RTFACT_LOG_LINE_F("Warning: %1%", (boost::format(aFormat) % aParams).str())

#endif // RTFACT_DEBUG

#pragma endregion // Warning logging macros

#pragma region Program warning macros

#ifdef RTFACT_NO_PROGRAM_WARNINGS

#   define RTFACT_PROGRAM_WARNING(aMessage)
#   define RTFACT_PROGRAM_WARNING_F(aFormat, aParams)
#   define RTFACT_PROGRAM_WARNING_IF(aCondition, aMessage)
#   define RTFACT_PROGRAM_WARNING_IF_F(aCondition, aFormat, aParams)

#else

#   define RTFACT_PROGRAM_WARNING(aMessage)                                    \
        RTFACT_LOG_WARNING(aMessage)

#   define RTFACT_PROGRAM_WARNING_F(aFormat, aParams)                          \
        RTFACT_LOG_WARNING_F(aFormat, aParams)

#   define RTFACT_PROGRAM_WARNING_IF(aCondition, aMessage)                     \
        if(aCondition) { RTFACT_PROGRAM_WARNING(aMessage); }

#   define RTFACT_PROGRAM_WARNING_IF_F(aCondition, aFormat, aParams)           \
        if(aCondition) { RTFACT_PROGRAM_WARNING_F(aFormat, aParams); }

#endif // RTFACT_NO_PROGRAM_WARNINGS

#pragma endregion // Program warning macros

#pragma region User warning macros

#ifdef RTFACT_NO_USER_WARNINGS

#   define RTFACT_USER_WARNING(aMessage)
#   define RTFACT_USER_WARNING_F(aFormat, aParams)
#   define RTFACT_USER_WARNING_IF(aCondition, aMessage)
#   define RTFACT_USER_WARNING_IF_F(aCondition, aFormat, aParams)

#else

#   define RTFACT_USER_WARNING(aMessage)                                       \
    RTFACT_LOG_WARNING(aMessage)

#   define RTFACT_USER_WARNING_F(aFormat, aParams)                             \
    RTFACT_LOG_WARNING_F(aFormat, aParams)

#   define RTFACT_USER_WARNING_IF(aCondition, aMessage)                        \
    if(aCondition) { RTFACT_USER_WARNING(aMessage); }

#   define RTFACT_USER_WARNING_IF_F(aCondition, aFormat, aParams)              \
    if(aCondition) { RTFACT_USER_WARNING_F(aFormat, aParams); }

#endif // RTFACT_NO_USER_WARNINGS

#pragma endregion // User warning macros

#pragma region Exception classes

class Exception
{
protected:

    std::string mMessage;
    std::string mFileName;
    int mFileLine;

    Exception(
        const std::string& aMessage,
        const char* aFileName,
        const int aLine
    ) :
        mMessage(aMessage),
        mFileName(aFileName),
        mFileLine(aLine)
    {}

public:

    const std::string& getMessage() const
    {
        return mMessage;
    }

    const std::string& getFileName() const
    {
        return mFileName;
    }

    int getLine() const
    {
        return mFileLine;
    }
};

class UserException : public Exception
{
public:
    UserException(
        const std::string& aMessage,
        const char *aFileName,
        const int aLine
    ) :
        Exception(aMessage, aFileName, aLine)
    {}

    UserException(
        const boost::format& aMessage,
        const char *aFileName,
        const int aLine
    ) :
        Exception(aMessage.str(), aFileName, aLine)
    {}
};


class ProgramException : public Exception
{
public:
    ProgramException(
        const std::string& aMessage,
        const char *aFileName,
        const int aLine
    ) :
        Exception(aMessage, aFileName, aLine)
    {}

    ProgramException(
        const boost::format& aMessage,
        const char *aFileName,
        const int aLine
    ) :
        Exception(aMessage.str(), aFileName, aLine)
    {}
};

#pragma endregion // Exception classes

#pragma region Error logging utils

#ifdef RTFACT__LOG_FULL

#define RTFACT_LOG_ERROR(aMessage)                                             \
    RTFACT_FULL_LOG_LINE_F("Error: %1%", (aMessage))

#define RTFACT_LOG_ERROR_F(aFormat, aParams)                                   \
    RTFACT_FULL_LOG_LINE_F("Error: %1%", (boost::format(aFormat) % aParams).str())

#else

#define RTFACT_LOG_ERROR(aMessage)                                             \
    RTFACT_LOG_LINE_F("Error: %1%", (aMessage))

#define RTFACT_LOG_ERROR_F(aFormat, aParams)                                   \
    RTFACT_LOG_LINE_F("Error: %1%", (boost::format(aFormat) % aParams).str())

#endif // RTFACT_DEBUG

#pragma endregion // Error logging utils

#pragma region Program error macros

#ifdef RTFACT_DEBUG

    #define RTFACT_PROGRAM_ERROR(aMessage)                                     \
        RTFACT_LOG_ERROR(aMessage);                                            \
        /* throw RTfact::ProgramException(aMessage, __FILE__, __LINE__); */    \
        RTFACT_BREAK_DEBUG();                                                  \
        RTFACT_BREAK_CONSOLE();                                                \

#else

    #define RTFACT_PROGRAM_ERROR(aMessage)                                     \
        RTFACT_LOG_ERROR(aMessage);                                            \
        RTFACT_LOG("Press any key to exit...");                                \
        RTFACT_BREAK_CONSOLE();                                                \
        exit(1);                                                               \

#endif

#define RTFACT_PROGRAM_ERROR_F(aFormat, aParams)                               \
    RTFACT_PROGRAM_ERROR((boost::format(aFormat) % aParams).str())

#define RTFACT_PROGRAM_ERROR_IF(aCondition, aMessage)                          \
    if(aCondition) { RTFACT_PROGRAM_ERROR(aMessage); }

#define RTFACT_PROGRAM_ERROR_IF_F(aCondition, aFormat, aParams)                \
    if(aCondition) { RTFACT_PROGRAM_ERROR_F(aFormat, aParams); }

#pragma endregion // Program error macros

#pragma region User error macros

#ifdef RTFACT_DEBUG

    #define RTFACT_USER_ERROR(aMessage)                                        \
        RTFACT_LOG_ERROR(aMessage);                                            \
        /* throw RTfact::UserException(aMessage, __FILE__, __LINE__); */       \
        RTFACT_BREAK_DEBUG();                                                  \
        RTFACT_BREAK_CONSOLE();                                                \

#else

    #define RTFACT_USER_ERROR(aMessage)                                        \
        RTFACT_LOG_ERROR(aMessage);                                            \
        RTFACT_LOG("Press any key to exit...");                                \
        RTFACT_BREAK_CONSOLE();                                                \
        exit(1);                                                               \

#endif

#define RTFACT_USER_ERROR_F(aFormat, aParams)                                  \
    RTFACT_USER_ERROR((boost::format(aFormat) % aParams).str())

#define RTFACT_USER_ERROR_IF(aCondition, aMessage)                             \
    if(aCondition) { RTFACT_USER_ERROR(aMessage); }

#define RTFACT_USER_ERROR_IF_F(aCondition, aFormat, aParams)                   \
    if(aCondition) { RTFACT_USER_ERROR_F(aFormat, aParams); }

#pragma endregion // User error macros

RTFACT_NAMESPACE_END

#ifdef RTFACT__LOG_FULL
#   undef RTFACT__LOG_FULL
#endif

#endif // RTFACT__ERRORLOGUTILS_HPP
