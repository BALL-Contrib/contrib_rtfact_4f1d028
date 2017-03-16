#ifndef RTANYSL_SINGLETON_HPP_INCLUDED
#define RTANYSL_SINGLETON_HPP_INCLUDED

#include <RTAnySL/Config/Common.hpp>
#include <cstdlib>
#include <stdexcept>

/* This code is based on the book:
 * Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
 * Patterns Applied". Copyright (c) 2001. Addison-Wesley.
 */

namespace RTAnySL
{

/** How to make a Singleton class
 *
 *  1) Declare your class as singleton by putting RTANYSL_DECLARE_SINGLETON
 *     or RTANYSL_DECLARE_DEFAULT_SINGLETON macro inside of the singleton class
 *     declaration.
 *     This will also make your class non-copyable by making copy constructor
 *     and assignment operator private.
 *
 *  2) Define private default constructor and destructor for your class.
 *
 *  class EXPORT_MACRO FooFactory
 *  {
 *      RTANYSL_DECLARE_DEFAULT_SINGLETON(FooFactory)
 *  public:
 *
 *      void doSomething();
 *
 *  private:
 *
 *      FooFactory() { }
 *
 *      ~FooFactory() { }
 *
 *  };
 *
 *  3) In cpp file put RTANYSL_DEFINE_SINGLETON macro.
 *
 *  RTANYSL_DEFINE_SINGLETON(FooFactory)
 *
 *  Use Singleton:
 *
 *  Singleton<FooFactory>::getInstance().doSomething();
 *
 */

// Default policies

#define RTANYSL_DEFAULT_CREATION_POLICY RTAnySL::CreateUsingNew
#define RTANYSL_DEFAULT_LIFETIME_POLICY RTAnySL::DefaultLifetime
#define RTANYSL_DEFAULT_THREADING_MODEL RTAnySL::MultiThreaded

// Threading policies

template <class T>
class SingleThreaded
{
public:
    typedef T VolatileType;
};

template <class T>
class MultiThreaded
{
public:
    typedef T VolatileType;
};

// Creation policies

template <class T>
class CreateUsingNew
{
public:

    static T* create()
    {
        return new T();
    }

    static void destroy(T* obj)
    {
        delete obj;
    }
};

// Lifetime policies

template <class T>
class DefaultLifetime
{
public:
    typedef void (*DeleteMethodType)();

    static void scheduleDestruction(DeleteMethodType d)
    {
        ::atexit(d);
    }

    static void onDeadReference()
    {
        throw std::logic_error("Dead Reference Detected");
    }
};

// Singleton

template <class T>
class Singleton
{
public:

    ///  Type of the singleton object
    typedef T ObjectType;

    static T& getInstance();

private:

    // Protection
    Singleton();

    // Helpers
    static void makeInstance();

    static void destroySingleton();

    typedef typename ObjectType::ThreadingModel::VolatileType InstancePtrType;

    static bool destroyed_;
};

/** Singleton Implementation */

/// Singleton::getInstance

template <class T>
inline T& Singleton<T>::getInstance()
{
    if (!T::instance_)
    {
        makeInstance();
    }
    return *T::instance_;
}


/// Singleton::makeInstance (helper for getInstance)

template <class T>
void Singleton<T>::makeInstance()
{
    if (!T::instance_)
    {
        if (T::destroyed_)
        {
            T::destroyed_ = false;
            T::LifetimePolicy::onDeadReference();
        }
        T::instance_ = T::CreationPolicy::create();
        T::LifetimePolicy::scheduleDestruction(&destroySingleton);
    }
}

template <class T>
void Singleton<T>::destroySingleton()
{
    T::CreationPolicy::destroy(T::instance_);
    T::instance_ = 0;
    T::destroyed_ = true;
}

/** Declares specified class as Singleton class */
#define RTANYSL_DECLARE_SINGLETON(CLASS_NAME, CREATION_POLICY,          \
                               LIFETIME_POLICY, THREADING_MODEL)        \
                                                                        \
protected:                                                              \
                                                                        \
    typedef CREATION_POLICY<CLASS_NAME> CreationPolicy;                 \
    typedef LIFETIME_POLICY<CLASS_NAME> LifetimePolicy;                 \
    typedef THREADING_MODEL<CLASS_NAME*> ThreadingModel;                \
    typedef ThreadingModel::VolatileType InstancePtrType;               \
                                                                        \
    friend class RTAnySL::Singleton<CLASS_NAME>;                        \
    friend class CREATION_POLICY<CLASS_NAME>;                           \
    friend class LIFETIME_POLICY<CLASS_NAME>;                           \
    friend class THREADING_MODEL<CLASS_NAME*>;                          \
                                                                        \
private:                                                                \
                                                                        \
    static InstancePtrType instance_;                                   \
    static bool destroyed_;                                             \
                                                                        \
    CLASS_NAME(const CLASS_NAME &);                                     \
    CLASS_NAME &operator=(const CLASS_NAME&);

#define RTANYSL_DEFINE_SINGLETON(CLASS_NAME)                            \
    CLASS_NAME::InstancePtrType CLASS_NAME::instance_ = 0;              \
    bool CLASS_NAME::destroyed_                       = false;

/** Declares specified class as Singleton class with default creation and
 *  lifetime policy, and threading model.
 */
#define RTANYSL_DECLARE_DEFAULT_SINGLETON(CLASS_NAME)           \
    RTANYSL_DECLARE_SINGLETON(CLASS_NAME,                       \
                              RTANYSL_DEFAULT_CREATION_POLICY,  \
                              RTANYSL_DEFAULT_LIFETIME_POLICY,  \
                              RTANYSL_DEFAULT_THREADING_MODEL)


} // namespace RTAnySL

#endif
