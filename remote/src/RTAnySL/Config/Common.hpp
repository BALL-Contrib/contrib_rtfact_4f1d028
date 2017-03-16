#ifndef RTANYSL_CONFIG_COMMON_HPP_INCLUDED
#define RTANYSL_CONFIG_COMMON_HPP_INCLUDED

// Determine Operating System
#ifdef _WIN32
#   define RTANYSL_OS_WIN
#elif defined(__CYGWIN__) ||                                                   \
      defined(__linux__) ||                                                    \
      defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) ||   \
      defined(__hpux) ||                                                       \
      defined(__sgi) ||                                                        \
      defined(__sun) ||                                                        \
      defined(_POSIX_SOURCE) || defined(_XOPEN_SOURCE)
#   define RTANYSL_OS_POSIX
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#   define RTANYSL_OS_MACOS
#else
#   pragma message("Warning: Cannot determine OS. Using default: POSIX")
#   define RTANYSL_OS_POSIX
#endif

#if defined(RTANYSL_OS_WIN)
#  if defined(RTANYSL_LIB)
#    define RTANYSL_API __declspec(dllexport)
#  else
#    define RTANYSL_API __declspec(dllimport)
#  endif
#else
#  define RTANYSL_API
#endif

#include <boost/cstdint.hpp>                // for integer typedefs

namespace RTAnySL
{

typedef boost::int8_t   int8;
typedef boost::int16_t  int16;
typedef boost::int32_t  int32;
typedef boost::int64_t  int64;

typedef boost::uint8_t  uint8;
typedef boost::uint16_t uint16;
typedef boost::uint32_t uint32;
typedef boost::uint64_t uint64;
typedef uint32          uint;

} // namespace RTAnySL

// Macros from Boost C++ Libraries

//
// Helper macro RTANYSL_STRINGIZE:
// Converts the parameter X to a string after macro replacement
// on X has been performed.
//
#define RTANYSL_STRINGIZE(X) RTANYSL_DO_STRINGIZE(X)
#define RTANYSL_DO_STRINGIZE(X) #X

//
// Helper macro RTANYSL_JOIN:
// The following piece of macro magic joins the two
// arguments together, even when one of the arguments is
// itself a macro (see 16.3.1 in C++ standard).  The key
// is that macro expansion of macro arguments does not
// occur in RTANYSL_DO_JOIN2 but does in RTANYSL_DO_JOIN.
//
#define RTANYSL_JOIN( X, Y ) RTANYSL_DO_JOIN( X, Y )
#define RTANYSL_DO_JOIN( X, Y ) RTANYSL_DO_JOIN2(X,Y)
#define RTANYSL_DO_JOIN2( X, Y ) X##Y

#endif // RTANYSL_CONFIG_COMMON_HPP_INCLUDED
