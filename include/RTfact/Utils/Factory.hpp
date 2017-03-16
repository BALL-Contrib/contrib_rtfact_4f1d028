/* 
 *  Factory.hpp
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
 
 #ifndef RTFACT__FACTORY_HPP
#define RTFACT__FACTORY_HPP

#include <RTfact/Config/Common.hpp>

#include <boost/intrusive_ptr.hpp>

#include <RTfact/Utils/Containers/HashMap.hpp>

RTFACT_NAMESPACE_BEGIN

#pragma region DefaultFactoryError policy class definition

template<
    class tIdentifier,
    class tCreator>
class DefaultFactoryError
{
    std::string mUnknownTypeErrorMessage;
    std::string mCreatorRegisterErrorMessage;
    std::string mCreatorUnregisterErrorMessage;

protected:

    typedef boost::intrusive_ptr<tCreator> CreatorPtr;

    DefaultFactoryError() :
        DefaultFactoryError(
            "Could not find a creator for class ID %s",
            "Could not register entity creator with ID %s",
            "Could not unregister entity creator with ID %s")
    {}

    DefaultFactoryError(
        const std::string& aUnknownTypeErrorMessage,
        const std::string& aCreatorRegisterErrorMessage,
        const std::string& aCreatorUnregisterErrorMessage
    ) :
        mUnknownTypeErrorMessage(aUnknownTypeErrorMessage),
        mCreatorRegisterErrorMessage(aCreatorRegisterErrorMessage),
        mCreatorUnregisterErrorMessage(aCreatorUnregisterErrorMessage)
    {}

    const bool onCreatorRegisterError(
        const tIdentifier& aID) const
    {
        RTFACT_USER_ERROR_F(boost::format(mCreatorRegisterErrorMessage), aID);

        return false;
    }

    const bool onCreatorUnregisterError(
        const tIdentifier& aID) const
    {
        RTFACT_USER_ERROR_F(boost::format(mCreatorUnregisterErrorMessage), aID);

        return false;
    }

    CreatorPtr onUnknownType(
        const tIdentifier& aID) const
    {
        RTFACT_USER_ERROR_F(boost::format(mUnknownTypeErrorMessage), aID);

        return RTFACT_NULL;
    }
};

#pragma endregion // DefaultFactoryError policy class definition

template <
    class tIdentifier,
    class tCreator,
    class tErrorPolicy = DefaultFactoryError<tIdentifier, tCreator>,
    class tCreatorContainer = HashMap<tIdentifier, boost::intrusive_ptr<tCreator> > >
class Factory :
    public tErrorPolicy
{
public:

    typedef boost::intrusive_ptr<tCreator> CreatorPtr;

private:

    tCreatorContainer mCreators;

public:

    virtual ~Factory()
    {}

    bool registerCreator(
        const tIdentifier& aID,
        CreatorPtr aCreator)
    {
        const int elementsInserted = mCreators.insert(
            typename tCreatorContainer::value_type(aID, aCreator)).second;

        return (elementsInserted == 1) ? true : onCreatorRegisterError(aID);
    }

    bool unregisterCreator(
        const tIdentifier& aID)
    {
        size_t elementsErased = mCreators.erase(aID);

        return (elementsErased == 1) ? true : onCreatorUnregisterError(aID);
    }

    CreatorPtr getCreator(
        const tIdentifier& aID)
    {
        typename tCreatorContainer::const_iterator it = mCreators.find(aID);
        
        if(it != mCreators.end())
        {
            return it->second;
        }
        else
        {
            return onUnknownType(aID);
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__FACTORY_HPP
