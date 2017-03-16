/*
 *  Synchronization.hpp
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
 *  Created on: 2011-02-25 10:53:00 +0100
 *  Author(s): Vaclav Krajicek
 */

#ifndef RTFACT__SYNCHRONIZATION_HPP
#define RTFACT__SYNCHRONIZATION_HPP

#include <RTfact/Config/Common.hpp>

RTFACT_NAMESPACE_BEGIN

class NullMutex
{
public:

    class scoped_lock
    {
    public:
        scoped_lock(NullMutex&) {}
        inline void acquire(NullMutex&) {}
        inline void try_acquire(NullMutex&) {}
        inline void release() {}
        static inline bool is_rw_mutex() { return true; }
    };

    static const bool is_recursive_mutex = true;
    static const bool is_fair_mutex = true;

    NullMutex()
    {}
};

RTFACT_NAMESPACE_END

#endif // RTFACT__SYNCHRONIZATION_HPP
