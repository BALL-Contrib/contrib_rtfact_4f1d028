/* 
 *  AccelBuildData.hpp
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
 *  Created on: 2009-02-24 16:01:56 +0100
 *  Author(s): Felix Klein, Petr Kmoch
 */
 
 #ifndef _RTFACT_ACCELBUILDDATA_HPP_INCLUDED
#define _RTFACT_ACCELBUILDDATA_HPP_INCLUDED


RTFACT_NAMESPACE_BEGIN

template <class tInnerStruct, class tAccelInstance>
class AccelBuildData
{
public:
    typedef tInnerStruct                        t_InnerStruct;
    typedef Vector<t_InnerStruct*>              t_StructVector;
    
    typedef tAccelInstance                      t_AccelInstance;
    typedef Vector<t_AccelInstance>             t_InstanceVector;

    t_StructVector      mInnerStructs;
    t_InstanceVector    mAccelInstances;

    ~AccelBuildData()
    {
        // Delete All InnerStructs once AccelBuildData is deleted.
        for(uint i=0; i<mInnerStructs.size(); i++)
            if(mInnerStructs[i] != NULL)
                delete mInnerStructs[i];
    }

};

RTFACT_NAMESPACE_END

#endif // _RTFACT_ACCELBUILDDATA_HPP_INCLUDED
