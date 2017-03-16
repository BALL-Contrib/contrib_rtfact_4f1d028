/* 
 *  StaticInitializer.hpp
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
 
 #ifndef RTFACT__STATIC_INITIALIZER_HPP
#define RTFACT__STATIC_INITIALIZER_HPP

#include <RTfact/Config/Common.hpp>

#include <set>
#include <boost/config/suffix.hpp>

#include <RTfact/Utils/Singleton.hpp>

RTFACT_NAMESPACE_BEGIN

class StaticInitializer :
    public Singleton<StaticInitializer>
{
    typedef void(*t_CallBack)();
    typedef std::set<t_CallBack> t_Container;

    t_Container mCallBacks;

    void performInitializationPrivate()
    {
        for(t_Container::iterator it = mCallBacks.begin();
            it != mCallBacks.end(); ++it)
        {
            (*it)();
        }
    }

    bool registerCallBackPrivate(
        t_CallBack aFunction)
    {
        mCallBacks.insert(aFunction);

        return true;
    }

public:

    static bool registerCallBack(
        t_CallBack aFunction)
    {
        return instance().registerCallBackPrivate(aFunction);
    }

    // returns bool only to be able to be called in global scope
    static bool performInitialization()
    {
        instance().performInitializationPrivate();

        return true;
    }

    RTFACT_SINGLETON_DECLARE_DEFAULT_MEMBERS_AND_CONSTRUCTORS(StaticInitializer)
};

#pragma region Instructions for performing static initialization

/*
    I) How to register a class for static initialization:

        1)  Define a static public method with the following prototype:

            static void initStatic();

        2)  Use the macro given below to register the class for static
            initialization;

        Example:

            class MyClass
            {
                ...

                static void initStatic()
                {
                    ...
                }

                ...
            };

            RTFACT_STATIC_INITIALIZER_REGISTER_CLASS(MyClass)

        The method MyClass::initStatic() will be called whenever
        StaticInitializer::performInitialization() is called (usually
        at the beginning of the main function).

    II) How to register an arbitrary function:

        void myFunc();

        RTFACT_STATIC_INITIALIZER_REGISTER_FUNCTION(&myFunc)
*/

#define RTFACT_STATIC_INITIALIZER_REGISTER_CLASS(aClassName)                   \
                                                                               \
    namespace                                                                  \
    {                                                                          \
        const bool                                                             \
        BOOST_JOIN(BOOST_JOIN(aClassName##_,__LINE__),_registered_for_static_init)\
        = ::RTfact::StaticInitializer::registerCallBack(&aClassName::initStatic);\
    }

#define RTFACT_STATIC_INITIALIZER_REGISTER_FUNCTION(aFunction)                 \
                                                                               \
    namespace                                                                  \
    {                                                                          \
        const bool                                                             \
        BOOST_JOIN(BOOST_JOIN(aFunction##_,__LINE__),_registered_for_static_init)\
            = ::RTfact::StaticInitializer::registerCallBack(aFunction);        \
    }

#pragma endregion // Instructions for performing static initialization

RTFACT_NAMESPACE_END

#endif // RTFACT__STATIC_INITIALIZER_HPP
