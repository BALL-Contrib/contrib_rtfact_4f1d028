#ifndef RTANYSL_LIBRARYINIT_HPP_INCLUDED
#define RTANYSL_LIBRARYINIT_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <boost/utility.hpp>

namespace RTAnySL
{

/**
 * Library initializer guard
 */
class RTANYSL_API LibraryInit : private boost::noncopyable
{
public:

    LibraryInit()
    {
        initialize();
    }

    ~LibraryInit()
    {
        shutdown();
    }

    static void initialize()
    {
        if (fetchAndAddCounter(1) == 0)
            initializeImpl();
    }

    static void shutdown()
    {
        if (fetchAndAddCounter(-1) == 1)
            shutdownImpl();
    }

private:

    // Note : not atomic counter, not multi-threading safe
    // Returns value before increment
    static int fetchAndAddCounter(int inc)
    {
        int tmp = counter;
        counter += inc;
        return tmp;
    }

    static int counter;

    static void initializeImpl();

    static void shutdownImpl();
};

} // namespace RTAnySL

#endif
