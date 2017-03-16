/* 
 *  RTImage.hpp
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
 *  Created on: 2008-11-27 16:51:44 +0100
 *  Author(s): Iliyan Georgiev, Felix Klein, Alexander Löffler
 */
 
 #ifndef RTFACT_REMOTE__IMAGE_HPP
#define RTFACT_REMOTE__IMAGE_HPP

#include <RTremote/Config/Common.hpp>
/*
#include <RTfact/Concept/Image/Image2D.hpp>
#include <RTfact/Utils/ReferenceCounted.hpp>
*/

RTFACT_REMOTE_NAMESPACE_BEGIN

class RTFACT_REMOTE_API RTImage :
    public ReferenceCounted
{
    friend class Renderer;
    friend class RTAppearance;

public:

    enum ComponentType
    {
        COMPONENT_FLOAT,
        COMPONENT_BYTE
    };

    virtual ComponentType getComponentType() const = 0;

    virtual int getComponentCount() const = 0;

    virtual int getWidth() const = 0;

    virtual int getHeight() const = 0;

    virtual void* getData() = 0;

    virtual size_t getDataLength() const = 0;

    virtual ~RTImage()
    {}
};

RTFACT_REMOTE_NAMESPACE_END

#endif // RTFACT_REMOTE__IMAGE_HPP

