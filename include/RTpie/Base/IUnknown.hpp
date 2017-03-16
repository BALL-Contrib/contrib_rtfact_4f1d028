/* 
 *  IUnknown.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2010  Saarland University
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
 *  Created on: 2010-09-26 00:35:31 +0200
 *  Author(s): Felix Klein
 */
 
 /*
 * IUnknown.h
 *
 *  Created on: 12.08.2010
 *      Author: Lachsen
 */

#ifndef RTFACT_RTPIE__IBASE_HPP
#define RTFACT_RTPIE__IBASE_HPP

#include <RTpie/Config/Common.hpp>

RTFACT_RTPIE_NAMESPACE_BEGIN


// {6ECC2085-A828-49f6-BC3D-B249ADEE79DE}
#define RTPIE_IUNKNOWN_ID \
		{ 0x58f55b3e, 0x8974, 0x4384, { 0x9a, 0x5e, 0x64, 0xf9, 0xc2, 0xfd, 0x41, 0xe } }


// 6608bb11-3711-4410-8824-b8031d57a3cc

/**
 * IUnknown Base Interface as known from COM
 * (should be at least binary compatible... maybe)
 */
struct IUnknown
{

	RTPIE_DECLARE_STATIC_IID_ACCESSOR(RTPIE_IUNKNOWN_ID)

    /**
     * Not implemented yet by any interface of RTpie
     */
    virtual HRESULT QueryInterface(REFIID, void **) = 0;

    /**
     * Increase the Reference Count
     * Reference Count is increased by default whenever
     * a pointer to an RTpie Interface is returned
     */
    virtual ULONG   AddRef() = 0;

    /**
     * Decrease the Reference Count
     * The Object is deleted once the reference count reaches 0
     */
    virtual ULONG   Release() = 0;
};

RTFACT_RTPIE_NAMESPACE_END


#endif /* IUNKNOWN_H_ */
