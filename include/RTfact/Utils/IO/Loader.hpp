/* 
 *  Loader.hpp
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
 *  Created on: 2008-11-06 02:30:21 +0100
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__LOADER_HPP
#define RTFACT__LOADER_HPP

#include <RTfact/Config/Common.hpp>

#include <string>

#include <boost/algorithm/string.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/cat.hpp>
#include <boost/preprocessor/seq/enum.hpp>

#include <RTfact/Utils/Factory.hpp>
#include <RTfact/Utils/Singleton.hpp>
#include <RTfact/Utils/IO/PathUtils.hpp>
#include <RTfact/Utils/IO/Configuration.hpp>

RTFACT_NAMESPACE_BEGIN

namespace IO {

#pragma region EntityFileLoader class definition

template<
    class tEntity>
class EntityFileLoader :
    public ReferenceCounted
{
public:

    typedef tEntity Entity;

    virtual Entity* load(
        const std::string& aPath,
        const Configuration& aConfig = Configuration())
    {
        RTFACT_USER_ERROR("Load functionality not implemented");

        return 0;
    }

    virtual void save(
        const Entity& aEntity,
        const std::string& aPath,
        const Configuration& aConfig = Configuration())
    {
        RTFACT_USER_ERROR("Save functionality not implemented");
    }
};

#pragma endregion // EntityFileLoader class definition

#pragma region LoaderError policy class definition

template<
    class tType>
class LoaderError :
    public DefaultFactoryError<std::string, EntityFileLoader<tType> >
{
    typedef DefaultFactoryError<std::string, EntityFileLoader<tType> > t_Base;

protected:

    LoaderError() :
         t_Base("Could not find a loader for type %s",
                "Could not register loader for type %s",
                "Could not unregister loader for type %s")
    {}
};

#pragma endregion // LoaderError policy class definition

#pragma region Loader static singleton factory class definition

template<
    class tType>
class Loader :
    public Singleton<Loader<tType> >,
    public Factory<std::string,
                   EntityFileLoader<tType>,
                   LoaderError<tType> >
{
    typedef Factory<std::string, EntityFileLoader<tType>, LoaderError<tType> > t_BaseFactory;
    typedef Singleton<Loader<tType> > t_BaseSingleton;

public:

    typedef typename t_BaseFactory::CreatorPtr       CreatorPtr;
    typedef typename EntityFileLoader<tType>::Entity Entity;

    using t_BaseFactory::getCreator;

    static bool registerLoader(
        const std::string& aFileExtention,
        CreatorPtr aLoader)
    {
        return t_BaseSingleton::instance().registerCreator(aFileExtention, aLoader);
    }

    static bool unregisterLoader(
        const std::string& aFileExtention)
    {
        return t_BaseSingleton::instance().unregisterCreator(aFileExtention);
    }

    static Entity* load(
        const std::string& aPath,
        const Configuration& aConfig = Configuration())
    {
        std::string::size_type n = aPath.rfind('.');

        RTFACT_USER_ERROR_IF_F(n == std::string::npos,
                                    "File does not have an extension: %s",
                                    aPath.c_str());

        CreatorPtr loader = t_BaseSingleton::instance().getCreator(std::string(
            boost::algorithm::to_lower_copy(aPath.substr(n))));

        return loader ? loader->load(aPath, aConfig) : 0;
    }

    static void save(
        const Entity& aEntity,
        const std::string& aPath,
        const Configuration& aConfig = Configuration())
    {
        std::string::size_type n = aPath.rfind('.');

        RTFACT_USER_ERROR_IF_F(n == std::string::npos,
            "File does not have an extension: %s",
            aPath.c_str());

        CreatorPtr loader = t_BaseSingleton::instance().getCreator(std::string(
            boost::algorithm::to_lower_copy(aPath.substr(n))));

        if (loader)
            loader->save(aEntity, aPath, aConfig);
    }

    RTFACT_SINGLETON_DECLARE_DEFAULT_MEMBERS_AND_CONSTRUCTORS(Loader<tType>)
};

#pragma endregion // Loader static singleton factory class definition

} // namespace IO

RTFACT_NAMESPACE_END

#pragma region Instructions for adding new loaders

/*
    How to add a new loader:

        1) Derive your loader from Loader<tType>

        2) Implement at least of one the virtual methods:

             Entity* load(
                const std::string& aPath,
                const Configuration& aConfig);

             void save(
                const Entity& aEntity,
                const std::string& aPath,
                const Configuration& aConfig);

        3) In your code (e.g. main function) either call
           Loader<tType>::registerLoader() or use one of the convenience macros
           below in global namespace. Do not put a call to the macro in header
           files as they might be included multiple times (by different source
           files).
    
    EXAMPLE:

            #include <Loader.hpp>

            class PlySceneLoader :
                public EntityFileLoader<Scene>
            {
                ...

            public:

                Entity* load(
                    const std::string& aPath,
                    const Configuration& aConfig)
                {
                    ...
                }

                void save(
                    const Entity& aEntity,
                    const std::string& aPath,
                    const Configuration& aConfig = Configuration())
                {
            };

            RTFACT_REGISTER_LOADER(SceneType, ".obj", ObjSceneLoader)
*/

#define RTFACT_REGISTER_LOADER(aType, aExtensionWithLeadingDot, aLoaderClassName)\
                                                                               \
namespace                                                                      \
{                                                                              \
    const bool aLoaderClassName##_registered_for_scene_loading                 \
        = Loader<aType>::instance().registerLoader(                            \
            aExtensionWithLeadingDot, new aLoaderClassName);                   \
}

#define RTFACT_REGISTER_TEMPLATE_LOADER(aType, aExtensionWithLeadingDot, aLoaderClassName, aLoaderParams)\
                                                                               \
namespace                                                                      \
{                                                                              \
    const bool BOOST_PP_CAT(aLoaderClassName##_registered_for_scene_loading_,  \
                            BOOST_PP_SEQ_CAT(aLoaderParams))                   \
        = Loader<aType >::instance().registerLoader(                           \
            aExtensionWithLeadingDot,                                          \
            new aLoaderClassName<BOOST_PP_SEQ_ENUM(aLoaderParams)>);           \
}

#pragma endregion

#endif // RTFACT__LOADER_HPP
