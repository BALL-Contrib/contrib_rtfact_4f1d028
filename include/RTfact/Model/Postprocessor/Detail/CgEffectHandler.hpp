/* 
 *  CgEffectHandler.hpp
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
 *  Created on: 2010-06-11 12:01:36 +0200
 *  Author(s): Lukas Marsalek
 */
 
 #ifndef RTFACT__CGEFFECTHANDLER_HPP
#define RTFACT__CGEFFECTHANDLER_HPP

#include <RTfact/Config/Common.hpp>

#include <boost/mpl/if.hpp>

#include <boost/type_traits/is_same.hpp>

#include <RTfact/Model/Postprocessor/Detail/Wiring.hpp>

#include <RTfact/Utils/ReferenceCounted.hpp>
#include <RTfact/Utils/IO/PathUtils.hpp>
#include <RTfact/Utils/Shaders/Includes.hpp>

RTFACT_NAMESPACE_BEGIN

template <
    typename tEffect>
class CgEffectHandler : public ReferenceCounted
{
    CGcontext mCgContext;
    CGeffect mCgEffect;
    CGtechnique mCgTechnique;

    void loadEffect(IO::PathList& aSearchPath)
    {
        assert(mCgEffect==NULL);		
        if(aSearchPath.empty())
        {
            mCgEffect=cgCreateEffectFromFile(mCgContext,tEffect::getFileName().c_str(),tEffect::getCompileArgs());
        }
        else
        {
            IO::PathList::const_iterator pathIt= aSearchPath.begin();
            while(pathIt != aSearchPath.end())
            {
                std::string effectPath = RTfact::IO::appendPath(*pathIt, tEffect::getFileName());
                mCgEffect=cgCreateEffectFromFile(mCgContext,effectPath.c_str(),tEffect::getCompileArgs());
                if (cgIsEffect(mCgEffect))
                {
                    break;
                }
                ++pathIt;
            }
        }
        if (!cgIsEffect(mCgEffect))
        {
            const char * const listing=cgGetLastListing(mCgContext);
            if (listing != NULL)
            {
                RTFACT_PROGRAM_WARNING(listing);
            }
            RTFACT_PROGRAM_ERROR("Error creating Cg effect in handler");
        }
        mCgTechnique=cgGetFirstTechnique(mCgEffect);
        while (
            mCgTechnique!=NULL && !cgValidateTechnique(mCgTechnique))
        {
            const char * const listing=cgGetLastListing(mCgContext);
            if (
                listing!=NULL)
            {
                RTFACT_PROGRAM_WARNING(listing);
            }
            mCgTechnique=cgGetNextTechnique(mCgTechnique);
        }
        RTFACT_PROGRAM_ERROR_IF(!cgIsTechnique(mCgTechnique),"No valid technique found in handler");
    }

    void prepareWiring()
    {
        mWiring=new Wiring::Effect(mCgContext,mCgEffect);
        
        for (
            CGpass passCg=cgGetFirstPass(mCgTechnique);
            passCg!=NULL;
            passCg=cgGetNextPass(passCg))
        {
            mWiring->addNewPass(passCg);
        }
    }

    void forgetCg()
    {
        mCgEffect=NULL;
        mCgTechnique=NULL;
        mWiring->forgetSelfContainedCg();
    }

public:
    typedef tEffect Effect;
    
    Traits<Wiring::Effect>::Ptr mWiring;
        //Postprocessor::init_initHandler
    std::vector<size_t> mPassId;
        //Postprocessor::createRenderEventLists
    tEffect mEffect;

    explicit CgEffectHandler(
        const CGcontext aContext)
    :
        mCgContext(aContext),
        mCgEffect(NULL),
        mCgTechnique(NULL)
    {
        assert(cgIsContext(aContext));
    }

    void init(IO::PathList aSearchPath)
    {
        loadEffect(aSearchPath);
        prepareWiring();
        cgDestroyEffect(mCgEffect);
        forgetCg();
    }

    void storePassIndices() {
        mPassId.resize(mWiring->mPassIdx.size());
        for (
            size_t idxMasterPass=0;idxMasterPass<mPassId.size();++idxMasterPass)
        {
            if (
                mWiring->mPassIdx[idxMasterPass]==size_t(-1))
            {
                mPassId[idxMasterPass]=-1;
            }
            else
            {
                mPassId[idxMasterPass]=mWiring->mPassIdx[idxMasterPass];
            }
        }
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__CGEFFECTHANDLER_HPP
