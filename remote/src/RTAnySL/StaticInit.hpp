#ifndef RTANYSL_STATICINIT_HPP_INCLUDED
#define RTANYSL_STATICINIT_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <cassert>
#include <boost/utility.hpp>

namespace RTAnySL
{

/** StaticInit is used to create a chain of initialization code
 *  similar or Java's static init. It is also similar to C atexit function
 *  which allow to create shutdown sequences.
 *
 *  RTAnySL::LibraryInit() function will call StaticInit::initialize() and
 *  activate initialization sequence.
 *
 */
class RTANYSL_API StaticInit : private boost::noncopyable
{
public:

    typedef void (*InitCallback)();

    /** Constructs new StaticInit object.
     *  Do not use it directly, instead use
     *  RTANYSL_REGISTER_STATIC_INIT_CALLBACK
     *  and RTANYSL_STATIC_INIT_FUNC macros.
     */
    explicit StaticInit(InitCallback initCB) : initCB(initCB), next(0)
    {
        appendThisToInitList();
    }

    ~StaticInit();

    /** run static initialization sequence */
    static void initialize();

private:

    typedef StaticInit * StaticInitPtr;

    StaticInit();

    static StaticInitPtr &getFirstRef();

    static StaticInitPtr &getLastRef();

    void appendThisToInitList();

    InitCallback initCB;
    StaticInit *next;
};

/** Run as argument passed function at static initialization time.
 *
 *  void static_init_func() { do_something(); ... }
 *
 *  RTANYSL_REGISTER_STATIC_INIT_CALLBACK(static_init_func);
 *
 */
#define RTANYSL_REGISTER_STATIC_INIT_CALLBACK(cb)                            \
    namespace                                                                \
    {                                                                        \
        RTAnySL::StaticInit RTANYSL_JOIN(rtanysl_auto_init_,__LINE__)(&cb);  \
    }

/** Declare inlined static initialization function e.g.
 *
 *  RTANYSL_STATIC_INIT_FUNC {
 *    do_static_initialization_stuff();
 *  }
 *
 */
#define RTANYSL_STATIC_INIT_FUNC                                             \
    static void RTANYSL_JOIN(rtanysl_auto_init_func_,__LINE__)(void);        \
    static RTAnySL::StaticInit RTANYSL_JOIN(rtanysl_auto_init_,__LINE__)     \
        (&RTANYSL_JOIN(rtanysl_auto_init_func_,__LINE__));                   \
    static void RTANYSL_JOIN(rtanysl_auto_init_func_,__LINE__)(void)

} // namespace RTAnySL

#endif
