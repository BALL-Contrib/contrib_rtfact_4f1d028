/* 
 *  Mailbox.hpp
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
 *  Created on: 2008-06-09 17:30:26 +0200
 *  Author(s): Iliyan Georgiev
 */
 
 #ifndef RTFACT__MAILBOX_HPP
#define RTFACT__MAILBOX_HPP

#include <RTfact/Config/Common.hpp>

#include <cstddef>

RTFACT_NAMESPACE_BEGIN

namespace Detail {

class RTFACT_ALIGN_CACHE_LINE Mailbox
{
    enum
    {
        SIZE = 32, // must be a power of 2
        HASH_MASK = SIZE - 1
    };

    const void * mData[SIZE];

public:

    Mailbox()
    {
        memset(mData, 0, SIZE * sizeof(void*));
    }

    RTFACT_FORCE_INLINE bool check(
        const void* const aObject) const
    { 
        return mData[reinterpret_cast<std::ptrdiff_t>(aObject) & HASH_MASK] ==
               aObject;
    }

    RTFACT_FORCE_INLINE void mark(
        const void* const aObject)
    { 
        mData[reinterpret_cast<std::ptrdiff_t>(aObject) & HASH_MASK] = aObject; 
    }
};

} // namespace Detail

RTFACT_NAMESPACE_END

#endif // RTFACT__MAILBOX_HPP
