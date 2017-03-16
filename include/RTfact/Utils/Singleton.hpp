/* 
 *  Singleton.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2008  Saarland University
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
 
 #ifndef RTFACT__SINGLETON_HPP
#define RTFACT__SINGLETON_HPP

#include <RTfact/Config/Common.hpp>

#include <string>

RTFACT_NAMESPACE_BEGIN

#pragma region Creation policies

template<
    class tType>
class NewSingletonCreationPolicy
{
    static tType* create()
    {
        return new tType;
    };

    static void destroy(
        tType* aPointer)
    {
        delete aPointer;
    }

    template<
        class tType1>
    friend class Singleton;
};

#pragma endregion // Creation policies

#pragma region Lifetime policies

template<
    class tType>
class StandardSingletonLifetimePolicy
{
    typedef void (*destructFunctionType)();

    static void scheduleDestruction(
        destructFunctionType aDestructFunction)
    {
        atexit(aDestructFunction);
    }

    static void onDeadReference()
    {
        RTFACT_PROGRAM_ERROR(tType::name() +
                           std::string(" instance has already been destructed"));
    }

    template<
        class tType1>
    friend class Singleton;
};

#pragma endregion // Lifetime policies

#pragma region Singleton class definition

template<
    class tType>
class Singleton
{
    typedef Singleton<tType> t_This;

    static bool mAlreadyDistroyed;
    static tType* mInstance;

    // Without definitions - should not be called
    Singleton(const t_This&);
    Singleton& operator=(const t_This&);

protected:

    Singleton()
    {}

    virtual ~Singleton()
    {}

public:

    static tType& instance()
    {
        if(!mInstance)
        {
            if(mAlreadyDistroyed)
            {
                tType::LifetimePolicy::onDeadReference();
                mAlreadyDistroyed = false;
            }

            mInstance = tType::CreationPolicy::create();
            tType::LifetimePolicy::scheduleDestruction(&destroyInstance);
        }

        return *mInstance;
    }

private:

    static void destroyInstance()
    {
        RTFACT_ASSERT(!mAlreadyDistroyed);

        tType::CreationPolicy::destroy(mInstance);

        mInstance = 0;
        mAlreadyDistroyed = true;
    }
};

template<
    class T>
bool Singleton<T>::mAlreadyDistroyed = false;

template<
    class T>
T* Singleton<T>::mInstance = 0;

#pragma endregion // Singleton class definition

#pragma region Instructions for declaring Singleton classes

/*
    How to make a class Singleton:

        1)  Derive your class from Singleton instantiated
            with the class itself. For example:

            class MyClass : public Singleton<MyClass> { ... };

        2)  Declare and define *private* default constructor
            and destructor of the class

        3)  Do not declare any public constructors or
            assignment operators

        4)  Put one of these macros at the end of the class definition

    Use:

        The only instance of the class is accessed in the following way:

        MyClass::instance()
*/

#define RTFACT_SINGLETON_DECLARE_MEMBERS(aClassName, aCreationPolicy,          \
                                                   aLifetimePolicy)            \
    private:                                                                   \
                                                                               \
        aClassName(const aClassName&);                                         \
        aClassName& operator=(const aClassName&);                              \
                                                                               \
    protected:                                                                 \
                                                                               \
        typedef aCreationPolicy<aClassName > CreationPolicy;                   \
        typedef aLifetimePolicy<aClassName > LifetimePolicy;                   \
                                                                               \
        friend class Singleton<aClassName >;                                   \
        friend class aCreationPolicy<aClassName >;                             \
        friend class aLifetimePolicy<aClassName >;                             \
                                                                               \
        static std::string name()                                              \
        {                                                                      \
            return #aClassName;                                                \
        }

#define RTFACT_SINGLETON_DECLARE_DEFAULT_MEMBERS(aClassName)                   \
    RTFACT_SINGLETON_DECLARE_MEMBERS(aClassName, NewSingletonCreationPolicy,   \
                                               StandardSingletonLifetimePolicy)

#define RTFACT_SINGLETON_DECLARE_DEFAULT_MEMBERS_AND_CONSTRUCTORS(aClassName)  \
    RTFACT_SINGLETON_DECLARE_DEFAULT_MEMBERS(aClassName)                       \
                                                                               \
    private:                                                                   \
        aClassName() {}                                                        \
        ~aClassName() {}

#pragma endregion // Instructions for declaring Singleton classes

RTFACT_NAMESPACE_END

#endif // RTFACT__SINGLETON_HPP
