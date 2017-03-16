/* 
 *  Wiring.hpp
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
 *  Author(s): Lukas Marsalek, Petr Kmoch, Ken Dahm
 */
 
 #ifndef RTFACT__POSTPROCESSOR_WIRING_HPP
#define RTFACT__POSTPROCESSOR_WIRING_HPP

#include <RTfact/Config/Common.hpp>

#include <map>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/foreach.hpp>
#include <boost/scoped_array.hpp>

#include <RTfact/Utils/ReferenceCounted.hpp>
#include <RTfact/Utils/Shaders/Includes.hpp>
#include <RTfact/Utils/Shaders/Parameters.hpp>

#include <RTfact/Model/Postprocessor/Detail/Metafunctions.hpp>

#ifdef RTFACT_DEBUG
    #include <fstream>    
#endif

RTFACT_NAMESPACE_BEGIN

typedef std::map<Shader::Semantic,size_t> SemanticStageMap;



/*
 * "Wiring" is the term used for C++ images of Cg entities (effects, passes, parameters).
 * An effect's wiring is a reflection of its Cg.
 */
namespace Wiring {

class Effect;
class Pass;
class Parameter;
class Varying;



class Varying :
    public ReferenceCounted
{
    Pass *mPass;
    CGtype mType;
    std::string mCgSemantic;

public:
    std::string mName;
        //Effect::mergeProgramParameters
    
    Varying(
        Pass &aPass,
        const CGparameter aParameter)
    :
        mPass(&aPass),
        mType(cgGetParameterType(aParameter)),
        mName(cgGetParameterName(aParameter)),
        mCgSemantic(cgGetParameterSemantic(aParameter))
    {
        RTFACT_ASSERT(cgIsParameter(aParameter));
        boost::to_upper(mCgSemantic);
    }
    
    Varying(
        Pass * const aPass,
        const Varying &aArg)
    :
        mPass(aPass),
        mType(aArg.mType),
        mName(aArg.mName),
        mCgSemantic(aArg.mCgSemantic)
    {}
    
    static const bool isUnitable(
        const Varying &aVar1,
        const Varying &aVar2)
    {
        if (
            aVar1.mName!=aVar2.mName)
        {
            return true;
        }
        else
        {
            return (
                aVar1.mType==aVar2.mType &&
                aVar1.mCgSemantic==aVar2.mCgSemantic);
        }
    }

    void generateCgCode(
        std::ostream &aOut) const
    {
        aOut<<"varying "<<cgGetTypeString(mType)<<" "<<mName<<":"<<mCgSemantic<<";"<<std::endl;
    }
};



class Parameter :
    public ReferenceCounted
{
    Pass *mPass;
    CGtype mType;

public:
    Shader::Semantic mSemantic;
        //Postprocessor::createMasterWiring,Effect::mergeProgramParameters
    size_t mStage;
        //Pass::isReady
    std::string mName;
        //Effect::mergeProgramParameters
    bool mForceTexture;
        //Effect::createMissingTextureParameters
    CGparameter mCgParameter;
        //Effect::wireCgEffectParameters

    Parameter() :
        mPass(NULL),
        mCgParameter(NULL),
        mSemantic(Shader::PARAM_IGNORE),
        mStage(0),
        mType(CG_UNKNOWN_TYPE),
        mForceTexture(false)
    {}
    
    Parameter(
        Pass &aPass,
        const CGparameter aCgParam,
        const Shader::Semantic aSemantic)
    :
        mPass(&aPass),
        mCgParameter(aCgParam),
        mSemantic(aSemantic),
        mStage(0),
        mType(cgGetParameterType(aCgParam)),
        mName(cgGetParameterName(aCgParam))
    {
        CGannotation annot=cgGetNamedParameterAnnotation(mCgParameter,"isTexture");
        int unused;
        if (
            annot!=NULL &&
            cgGetAnnotationType(annot)==CG_BOOL &&
            cgGetBoolAnnotationValues(annot,&unused)[0]
        ) {
            mForceTexture=true;
        }
        else
        {
            mForceTexture=false;
        }
    }
    
    Parameter(
        Pass *aPass,
        const Parameter &aParameter)
    :
        mPass(aPass),
        mCgParameter(aParameter.mCgParameter),
        mSemantic(aParameter.mSemantic),
        mStage(aParameter.mStage),
        mType(aParameter.mType),
        mName(aParameter.mName),
        mForceTexture(aParameter.mForceTexture)
    {}
    
    static Traits<Parameter>::Ptr createTextureParameter(
        const Parameter &aParam)
    {
        Parameter *res=new Parameter();
        res->mSemantic=aParam.mSemantic;
        res->mType=CG_SAMPLERRECT;
        return res;
    }
    
    void setStage(
        size_t aStage)
    {
        mStage=aStage;
    }
    
    static const bool isUnitable(
        const Parameter &aParam1,
        const Parameter &aParam2)
    {
        if (
            aParam1.mName!=aParam2.mName)
        {
            return true;
        }
        else
        {
            if (
                aParam1.mType==aParam2.mType &&
                aParam1.mSemantic==aParam2.mSemantic)
            {
                if (
                    aParam1.isTexture())
                {
                    //ToDo: verify unitability of sampler states, if applicable
                    return true;
                }
                else
                {
                    return true;
                }
            }
            else
            {
                return false;
            }
        }
    }
    
    static const bool isConnectable(
        const Parameter &aEffectParam,
        const Parameter &aProgParam)
    {
        return aEffectParam.mType==aProgParam.mType;
        //ToDo: consider if sampler states affect this
    }
    
    bool isTexture() const {
        return cgGetTypeClass(mType)==CG_PARAMETERCLASS_SAMPLER;
    }
    
    void generateCgCode(
        std::ostream &aOut) const
    {
        aOut<<cgGetTypeString(mType)<<" "<<mName;
        if (
            isTexture())    //ToDo:temporary
        {
            aOut<<" = sampler_state { MinFilter = Nearest; MagFilter = Nearest; GenerateMipMap = false; }";
        }
        aOut<<";"<<std::endl;
    }
    
    template <
        bool taFailOnError>
    void updateCgParameter(
        const CGeffect aEffect)
    {
        mCgParameter=cgGetNamedEffectParameter(aEffect,mName.c_str());
        RTFACT_ASSERT(!(taFailOnError && mCgParameter==NULL));
    }
    
    void forgetSelfContainedCg()
    {
        mCgParameter=NULL;
    }

    void saveXML(
        std::ostream &aOut) const
    {
        using std::endl;
        aOut<<"<Parameter>"<<endl;
        
        if (!mName.empty()) aOut<<"<Name val='"<<mName<<"' />"<<endl;
        if (mCgParameter!=NULL) aOut<<"<CgParameter />"<<endl;
        aOut<<"<Semantic sem='"<<Shader::semantic2str(mSemantic)<<"' />"<<endl;
        if (mPass!=NULL) aOut<<"<Pass />"<<endl;
        aOut<<"<Type name='"<<cgGetTypeString(mType)<<"' />"<<endl;
        aOut<<"<Stage idx='"<<mStage<<"' />"<<endl;
        if (mForceTexture) aOut<<"<ForceTexture />"<<endl;

        aOut<<"</Parameter>"<<endl;
    }
};

class Pass :
    public ReferenceCounted
{
    std::set<Shader::Semantic> mInputSingles,mInputTextures;
    std::string mOutputSwizzle;

    typedef boost::fusion::vector<float,int,bool> CgTypes;
    

    template <typename tType>
    struct MakeMap {
        typedef std::map<std::string,std::vector<tType> > type;
    };
    typedef boost::mpl::transform<CgTypes,MakeMap<mpl_::_1> >::type StateMaps;
    
    StateMaps mStateMaps;
    
    typedef std::map<Shader::Semantic,StateMaps> SemanticStateMap;
    SemanticStateMap mInputStateMap;



    typedef boost::fusion::zip_view<boost::fusion::vector<StateMaps &, const StateMaps &> > TT;

    struct copyFrom_addState
    {
        const std::string &mName;
        const CGtype mCgType;
        const CGstateassignment mAssi;
        copyFrom_addState(const std::string &aName,const CGtype aCgType,const CGstateassignment aAssi) : mName(aName),mCgType(aCgType),mAssi(aAssi) {}
        
        template <typename tMap>
        void operator() (
            tMap &aMap) const
        {}
    };

    struct isStateCompatible_and
    {
        typedef bool result_type;
        
        template <
            typename tPair>
        bool operator() (
            const bool aSoFar,
            const tPair &aPair) const
        {
            if (!aSoFar) return false;
            
            std::set<std::string> tested;
            typedef typename boost::fusion::tuple_element<0,tPair>::type MapRef;
            BOOST_STATIC_ASSERT(boost::is_reference<MapRef>::value);
            typedef typename boost::remove_reference<MapRef>::type Map;
            const Map &map1=boost::fusion::get<0>(aPair),&map2=boost::fusion::get<1>(aPair);
            typedef typename Map::iterator It;
            
            BOOST_FOREACH(
                typename Map::const_reference entry1,
                map1)
            {
                typename Map::const_iterator it2=map2.find(entry1.first);
                if (
                    it2!=map2.end())
                {
                    if (
                        entry1.second!=it2->second)
                    {
                        return false;
                    }
                }
                tested.insert(entry1.first);
            }
            
            BOOST_FOREACH(
                typename Map::const_reference entry2,
                map2)
            {
                if (
                    tested.find(entry2.first)==tested.end())
                {
                    typename Map::const_iterator it1=map1.find(entry2.first);
                    if (
                        it1!=map1.end())
                    {
                        if (
                            entry2.second!=it1->second)
                        {
                            return false;
                        }
                    }
                }
            }
            
            return true;
        }
    };

    struct append_mergeToLeft
    {
        template <
            typename tPair>
        void operator() (const tPair &aPair) const
        {
            typedef typename boost::fusion::tuple_element<0,tPair>::type MapRef;
            BOOST_STATIC_ASSERT(boost::is_reference<MapRef>::value);
            typedef typename boost::remove_reference<MapRef>::type Map;
            Map &left=boost::fusion::get<0>(aPair);
            const Map &right=boost::fusion::get<1>(aPair);
            left.insert(right.begin(),right.end());
        }
    };
    
    struct generateStateAssignmentCgCode_generate
    {
        std::ostream &mOut;
        generateStateAssignmentCgCode_generate(std::ostream &aOut) : mOut(aOut) {}
        
        template <
            typename tMap>
        void operator() (
            const tMap &aMap) const
        {
            BOOST_FOREACH(
                typename tMap::const_reference entry,
                aMap)
            {
                mOut<<entry.first<<"=";
                if (
                    entry.second.size()>1)
                {
                    mOut<<getCgTypeName<typename tMap::mapped_type::value_type>(entry.second.size())<<"(";
                    for (
                        typename tMap::mapped_type::const_iterator itVal=entry.second.begin();
                        itVal!=entry.second.end();
                        ++itVal)
                    {
                        if (
                            itVal!=entry.second.begin())
                        {
                            mOut<<",";
                        }
                        mOut<<*itVal;
                    }
                    mOut<<");";
                }
                else
                {
                    mOut<<entry.second.front()<<";";
                }
                mOut<<std::endl;
            }
        }
    };
    

    static const std::string getCgTypeName(
        boost::format &aFormat,
        const size_t aSize)
    {
        if (
            aSize==1)
        {
            return (aFormat%"").str();
        }
        else if (
            aSize==16)
        {
            return (aFormat%"4x4").str();
        }
        else
        {
            return (aFormat%aSize).str();
        }
    }

    template <
        typename tType>
    static const std::string getCgTypeName(
        const size_t aSize)
    {
        return "BAD_CG_TYPE_NAME_REQUESTED";
    }

    Traits<Parameter>::Ptr getInputParameter(
        const Shader::Semantic aSemantic) const
    {
        //ToDo:optimize
        BOOST_FOREACH(
            Traits<Parameter>::Ptr param,
            mInputs)
        {
            if (
                param->mSemantic==aSemantic)
            {
                return param;
            }
        }
        return NULL;
    }

public:
    CGprogram mCgProgram;
        //Effect::addNewPass
    std::vector<Traits<Parameter>::Ptr> mInputs;
        //Postprocessor::createMasterWiring
    Shader::Semantic mOutput;
        //Postprocessor::createMasterWiring
    std::vector<const Pass*> mMirroredPasses;
        //Effect::createCg
    Effect &mEffect;
        //Effect::createCg
    std::vector<Shader::Semantic> mSilentReads;
        //Effect::createCg
    std::string mProgramEntryPoint;
        //Effect::createCg
    size_t mPassIdx;
        //Effect::indexPasses
    CGpass mCgPass;
        //Postprocessor::render
    std::list<Traits<Varying>::Ptr> mVaryings;
        //Effect::mergeProgramParameters

    struct Reads :
        public std::unary_function<Traits<Pass>::Ptr,bool>
    {
        Shader::Semantic mSemantic;
        explicit Reads(Shader::Semantic aSem) : mSemantic(aSem) {}
        bool operator() (Traits<Pass>::Ptr aPass) {
            return aPass->hasInput(mSemantic);
        }
    };

    explicit Pass(
        Effect &aEffect,
        const size_t aIdx)
    :
        mEffect(aEffect),
        mPassIdx(aIdx)
    {}

    void copyFrom(
        const CGpass aPass)
    {
        mCgPass=aPass;
        CGannotation annot=cgGetNamedPassAnnotation(aPass,"output");
        RTFACT_USER_ERROR_IF(annot==NULL,"Pass definition missing 'output' annotation");
        RTFACT_USER_ERROR_IF(cgGetAnnotationType(annot)!=CG_STRING,"Pass 'output' annotation is not string");
        mOutput=Shader::str2semantic(cgGetStringAnnotationValue(annot));
        annot=cgGetNamedPassAnnotation(aPass,"outSwizzle");
        if (
            annot!=NULL)
        {
            RTFACT_USER_ERROR_IF(cgGetAnnotationType(annot)!=CG_STRING,"Pass 'outSwizzle' annotation is not string");
            mOutputSwizzle=".";
            mOutputSwizzle.append(cgGetStringAnnotationValue(annot));
        }
        else
        {
            mOutputSwizzle.clear();
        }
        
        mCgProgram=cgGetPassProgram(aPass,CG_FRAGMENT_DOMAIN);
        if (
            !cgIsProgram(mCgProgram))
        {
            const char * const listing=cgGetLastListing(cgGetEffectContext(cgGetTechniqueEffect(cgGetPassTechnique(aPass))));
            if (
                listing!=NULL)
            {
                RTFACT_PROGRAM_WARNING(listing);
            }
            RTFACT_USER_ERROR("Pass without fragment program");
        }
        mProgramEntryPoint=cgGetProgramString(mCgProgram,CG_PROGRAM_ENTRY);
        
        for (
            CGparameter param=cgGetFirstParameter(mCgProgram,CG_GLOBAL);
            param!=NULL;
            param=cgGetNextParameter(param))
        {
            if (
                cgGetParameterDirection(param)!=CG_OUT &&
                cgIsParameterUsed(param,mCgProgram))
            {
                if (
                    cgGetParameterVariability(param)==CG_VARYING)
                {
                    mVaryings.push_back(new Varying(*this,param));
                }
                else
                {
                    Shader::Semantic sem=Shader::str2semantic(cgGetParameterSemantic(param),cgGetParameterName(param));
                    if (
                        sem!=Shader::PARAM_IGNORE)
                    {
                        Parameter *par=new Parameter(*this,param,sem);
                        mInputs.push_back(par);
                        if (
                            par->isTexture())
                        {
                            RTFACT_USER_ERROR_IF(!mInputTextures.insert(sem).second,"Two texture parameters with same semantic not implemented yet");
                            for (
                                CGstateassignment assi=cgGetFirstSamplerStateAssignment(param);
                                assi!=NULL;
                                assi=cgGetNextStateAssignment(assi))
                            {
                                CGstate state=cgGetStateAssignmentState(assi);
                                std::string stateName=cgGetStateName(state);
                                CGtype cgType=cgGetTypeBase(cgGetStateType(state));
                                boost::fusion::for_each(mInputStateMap[sem],copyFrom_addState(stateName,cgType,assi));
                            }
                        }
                        else
                        {
                            mInputSingles.insert(sem);
                        }
                    }
                }
            }
        }
        for (
            CGparameter param=cgGetFirstParameter(mCgProgram,CG_PROGRAM);
            param!=NULL;
            param=cgGetNextParameter(param))
        {
            RTFACT_USER_ERROR_IF(
                cgGetParameterDirection(param)!=CG_OUT &&
                cgIsParameterUsed(param,mCgProgram) &&
                cgGetParameterVariability(param)!=CG_VARYING,
                "Program non-varying input parameters not implemented yet");
        }
        
        for (
            CGstateassignment assi=cgGetFirstStateAssignment(aPass);
            assi!=NULL;
            assi=cgGetNextStateAssignment(assi))
        {
            CGstate state=cgGetStateAssignmentState(assi);
            std::string stateName=cgGetStateName(state);
            CGtype rawType=cgGetStateType(state);
            if (
                rawType==CG_ARRAY)
            {
                stateName=(boost::format("%1%[%2%]")%stateName%cgGetStateAssignmentIndex(assi)).str();
            }
            CGtype cgType=cgGetTypeBase(rawType);
            boost::fusion::for_each(mStateMaps,copyFrom_addState(stateName,cgType,assi));
        }
    }

    void forgetSelfContainedCg()
    {
        mCgPass=NULL;
        mCgProgram=NULL;
        BOOST_FOREACH(
            Traits<Parameter>::Ptr input,
            mInputs)
        {
            input->forgetSelfContainedCg();
        }
    }

    void addInputCopy(
        //accessed from Effect::createMissingTextureParameters
        const Parameter &aParam)
    {
        mInputs.push_back(new Parameter(this,aParam));
        if (
            aParam.isTexture())
        {
            mInputTextures.insert(aParam.mSemantic);
        }
        else
        {
            mInputSingles.insert(aParam.mSemantic);
        }
    }

    void mirror(
        const Pass &aPass)
    {
        mOutput=aPass.mOutput;
        mCgPass=aPass.mCgPass;
        mCgProgram=aPass.mCgProgram;
        mProgramEntryPoint=aPass.mProgramEntryPoint;
        mInputs.reserve(aPass.mInputs.size());
        BOOST_FOREACH(
            Traits<Parameter>::Ptr param,
            aPass.mInputs)
        {
            addInputCopy(*param);
        }
        mVaryings.clear();
        BOOST_FOREACH(
            Traits<Varying>::Ptr var,
            aPass.mVaryings)
        {
            mVaryings.push_back(new Varying(this,*var));
        }
        mInputSingles=aPass.mInputSingles;
        mInputTextures=aPass.mInputTextures;
        mInputStateMap=aPass.mInputStateMap;
        mStateMaps=aPass.mStateMaps;
        mMirroredPasses.push_back(&aPass);
    }

    void append(
        const Pass &aPass)
    {
        mOutput=aPass.mOutput;
        mMirroredPasses.push_back(&aPass);
        BOOST_FOREACH(
            Traits<Parameter>::Ptr param,
            aPass.mInputs)
        {
            if (
                param->isTexture())
            {
                mInputTextures.insert(param->mSemantic);;
                mInputs.push_back(new Parameter(this,*param));
            }
            else
            {
                mInputSingles.insert(param->mSemantic);
                mInputs.push_back(new Parameter(this,*param));
            }
        }
        
        BOOST_FOREACH(
            Traits<Varying>::Ptr param,
            aPass.mVaryings)
        {
            mVaryings.push_back(new Varying(this,*param));
        }
        
        boost::fusion::for_each(zipPair(mStateMaps,aPass.mStateMaps),append_mergeToLeft());
        std::set<Shader::Semantic> merged;
        BOOST_FOREACH(
            SemanticStateMap::reference entryL,
            mInputStateMap)
        {
            SemanticStateMap::const_iterator itR=aPass.mInputStateMap.find(entryL.first);
            if (
                itR!=aPass.mInputStateMap.end())
            {
                boost::fusion::for_each(zipPair(entryL.second, itR->second),append_mergeToLeft());
            }
            merged.insert(entryL.first);
        }
        BOOST_FOREACH(
            SemanticStateMap::const_reference entryR,
            aPass.mInputStateMap)
        {
            if (
                merged.find(entryR.first)==merged.end())
            {
                mInputStateMap[entryR.first]=entryR.second;
            }
        }
        
        mCgPass=NULL;
        mCgProgram=NULL;
        mProgramEntryPoint.clear();
    }

    const bool isReady(
        const SemanticStageMap &aStageMap) const
    {
        BOOST_FOREACH(
            Traits<Parameter>::Ptr param,
            mInputs)
        {
            if (
                param->mStage > aStageMap.find(param->mSemantic)->second)
            {
                return false;
            }
        }
        return true;
    }
    
    static bool isStateCompatible(
        const Pass &aPass1,
        const Pass &aPass2)
    {
        if (
            !boost::fusion::accumulate(zipPair(aPass1.mStateMaps,aPass2.mStateMaps),true,isStateCompatible_and()))
        {
            return false;
        }
        std::set<Shader::Semantic> tested;

        BOOST_FOREACH(
            SemanticStateMap::const_reference entry1,
            aPass1.mInputStateMap)
        {
            SemanticStateMap::const_iterator it2=aPass2.mInputStateMap.find(entry1.first);
            if (
                it2!=aPass2.mInputStateMap.end())
            {
                if (
                    !boost::fusion::accumulate(zipPair(entry1.second,it2->second),true,isStateCompatible_and()))
                {
                    return false;
                }
            }
            tested.insert(entry1.first);
        }

        BOOST_FOREACH(
            SemanticStateMap::const_reference entry2,
            aPass2.mInputStateMap)
        {
            if (
                tested.find(entry2.first)!=tested.end())
            {
                SemanticStateMap::const_iterator it1=aPass1.mInputStateMap.find(entry2.first);
                if (
                    it1!=aPass1.mInputStateMap.end())
                {
                    if (
                        !boost::fusion::accumulate(zipPair(it1->second,entry2.second),true,isStateCompatible_and()))
                    {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    void computeSilentReads()
    {
        mSilentReads.clear();
        mSilentReads.resize(std::min(mInputSingles.size(),mInputTextures.size()));
        std::vector<Shader::Semantic>::iterator newEnd=std::set_intersection(
            mInputSingles.begin(),mInputSingles.end(),
            mInputTextures.begin(),mInputTextures.end(),
            mSilentReads.begin());
        mSilentReads.erase(newEnd,mSilentReads.end());
    }

    void generateMainBodyCgCode(
        std::ostream &aOut) const
    {
        std::vector<const Pass*>::const_iterator itWriterPass,itReaderPass;
        itWriterPass=mMirroredPasses.begin();
        itReaderPass=boost::next(itWriterPass);
        while (
            itWriterPass!=mMirroredPasses.end())
        {
            if (
                itReaderPass!=mMirroredPasses.end())
            {
                Traits<Parameter>::Ptr input=(*itReaderPass)->getInputParameter((*itWriterPass)->mOutput);
                if (
                    input!=NULL)
                {
                    aOut<<input->mName<<" = ";
                }
            }
            else
            {
                aOut<<"return ";
            }
            aOut<<(*itWriterPass)->mProgramEntryPoint<<"()";
            if (
                itReaderPass!=mMirroredPasses.end())
            {
                aOut<<(*itWriterPass)->mOutputSwizzle;
            }
            aOut<<";"<<std::endl;
            ++itWriterPass;
            if (
                itReaderPass!=mMirroredPasses.end())
            {
                ++itReaderPass;
            }
        }
    }

    void generateStateAssignmentCgCode(
        std::ostream &aOut) const
    {
        boost::fusion::for_each(mStateMaps,generateStateAssignmentCgCode_generate(aOut));
    }

    void updateCgPass(
        const CGtechnique aTechnique,
        const size_t aIdxPass)
    {
        mCgPass=cgGetNamedPass(aTechnique,(boost::format("%1%%2%")%Shader::EFFECT_PASS_NAME%aIdxPass).str().c_str());
        assert(mCgPass!=NULL);
    }

    void storeRequiredTextures(
        unsigned short * const aTextures) const
    {
        BOOST_FOREACH(
            Traits<Parameter>::Ptr param,
            mInputs)
        {
            if (
                param->isTexture())
            {
                if (
                    param->mSemantic==mOutput)
                {
                    aTextures[param->mSemantic]=2;
                }
                else if (
                    aTextures[param->mSemantic]==0)
                {
                    aTextures[param->mSemantic]=1;
                }
            }
        }
        if (
            aTextures[mOutput]==0)
        {
            aTextures[mOutput]=1;
        }
    }

    const bool hasInput(
        const Shader::Semantic aSemantic) const
    {
        return (
            mInputSingles.find(aSemantic)!=mInputSingles.end() ||
            mInputTextures.find(aSemantic)!=mInputTextures.end());
    }
    
    const bool hasTextureInput(
        const Shader::Semantic aSemantic) const
    {
        return mInputTextures.find(aSemantic)!=mInputTextures.end();
    }

    void saveXML(
        std::ostream &aOut) const
    {
        using std::endl;
        aOut<<"<Pass>"<<endl;
        
        if (mCgProgram!=NULL) aOut<<"<CgProgram />"<<endl;
        if (mCgPass!=NULL) aOut<<"<CgPass />"<<endl;
        if (!mProgramEntryPoint.empty()) aOut<<"<ProgramEntryPoint name='"<<mProgramEntryPoint<<"' />"<<endl;
        aOut<<"<PassIdx idx='"<<mPassIdx<<"' />";

        if (
            !mInputs.empty())
        {
            aOut<<"<Inputs>"<<endl;
            BOOST_FOREACH(
                Traits<Parameter>::Ptr param,
                mInputs)
            {
                param->saveXML(aOut);
            }
            aOut<<"</Inputs>"<<endl;
        }
        
        if (
            !mInputSingles.empty())
        {
            aOut<<"<InputSingles>"<<endl;
            BOOST_FOREACH(
                Shader::Semantic sem,
                mInputSingles)
            {
                aOut<<"<Input semantic='"<<Shader::semantic2str(sem)<<"' />"<<endl;
            }
            aOut<<"</InputSingles>"<<endl;
        }
        
        if (
            !mInputTextures.empty())
        {
            aOut<<"<InputTextures>"<<endl;
            BOOST_FOREACH(
                Shader::Semantic sem,
                mInputTextures)
            {
                aOut<<"<Input semantic='"<<Shader::semantic2str(sem)<<"' />"<<endl;
            }
            aOut<<"</InputTextures>"<<endl;
        }
        
        aOut<<"<Output semantic='"<<Shader::semantic2str(mOutput)<<"' />"<<endl;
        aOut<<"<OutputSwizzle string='"<<mOutputSwizzle<<"' />"<<endl;
        
        if (
            !mMirroredPasses.empty())
        {
            aOut<<"<MirroredPasses>"<<endl;
            BOOST_FOREACH(
                const Pass *pass,
                mMirroredPasses)
            {
                pass->saveXML(aOut);
            }
            aOut<<"</MirroredPasses>"<<endl;
        }
        
        if (
            !mSilentReads.empty())
        {
            aOut<<"<SilentReads>"<<endl;
            BOOST_FOREACH(
                Shader::Semantic sem,
                mSilentReads)
            {
                aOut<<"<Read semantic='"<<Shader::semantic2str(sem)<<"' />"<<endl;
            }
            aOut<<"</SilentReads>"<<endl;
        }

        aOut<<"</Pass>"<<endl;
    }
};

struct castToBool : public std::unary_function<CGbool,bool>
{
    bool operator() (
        CGbool aValue) const
    {
        return aValue==CG_TRUE;
    }
};

template <>
RTFACT_INLINE void Pass::copyFrom_addState::operator() (
    MakeMap<bool>::type &aMap) const
{
    if (
        mCgType==CG_BOOL)
    {
        int numValues;
        const CGbool * const val=cgGetBoolStateAssignmentValues(mAssi,&numValues);
        MakeMap<bool>::type::mapped_type &values=aMap[mName];

        values.resize(numValues);
        std::transform(
            val,
            val+numValues,
            values.begin(),
            castToBool()
        );
    }
}

#define PROCESS_TYPE(maCgType,maType,maTypeName) \
template <>\
RTFACT_INLINE void Pass::copyFrom_addState::operator() (\
    MakeMap<maType>::type &aMap) const\
{\
    if (\
        mCgType==maCgType)\
    {\
        int numValues;\
        const maType * const val=cgGet##maTypeName##StateAssignmentValues(mAssi,&numValues);\
        aMap[mName].assign(val,val+numValues);\
    }\
}

PROCESS_TYPE(CG_FLOAT,float,Float)
PROCESS_TYPE(CG_INT,int,Int)
//CGbool is an int, so the above cannot be used for it (and expanding the definition to use CGbool results in warning 4800 on MSVC)
#undef PROCESS_TYPE

template <>
RTFACT_INLINE const std::string Pass::getCgTypeName<float>(
    const size_t aSize)
{
    boost::format format("float%1%");
    return getCgTypeName(format,aSize);
}

template <>
RTFACT_INLINE const std::string Pass::getCgTypeName<int>(
    const size_t aSize)
{
    boost::format format("int%1%");
    return getCgTypeName(format,aSize);
}

template <>
RTFACT_INLINE const std::string Pass::getCgTypeName<bool>(
    const size_t aSize)
{
    boost::format format("bool%1%");
    return getCgTypeName(format,aSize);
}




class Effect : public ReferenceCounted
{
    CGcontext mCgContext;
    CGeffect mCgEffect;
    CGtechnique mCgTechnique;
    std::string mSourceCode;
    typedef std::map<std::string,Traits<Parameter>::Ptr> ParameterNameMap;
    ParameterNameMap mProgramParameterNameMap;  //pointers into passes
    typedef std::map<std::string,Traits<Varying>::Ptr> VaryingNameMap;
    VaryingNameMap mProgramVaryingNameMap;  //pointers into passes
    struct EffectParameter
    {
        Traits<Parameter>::Ptr mEffectParameter;    //owning pointer
        std::list<Traits<Parameter>::Ptr> mConnectedParameters; //pointer into pass
        EffectParameter() : mEffectParameter(NULL) {}
        explicit EffectParameter(Traits<Parameter>::Ptr aParam) : mEffectParameter(aParam) {}
    };
    typedef std::map<Shader::Semantic,EffectParameter> EffectParameterMap;
    EffectParameterMap mEffectParameterMap,mEffectTexParameterMap;
    //std::list<Traits<Varying>::Ptr> mEffectVaryings;    //pointers into passes

    static std::string extractFunctions(
        const std::string &aCode)
    {
        bool copy=false;
        std::istringstream str(aCode);
        std::ostringstream res;
        const size_t bufLen=aCode.length()+1;
        boost::scoped_array<char> buf(new char[bufLen]);
        while (
            !str.eof())
        {
            str.getline(buf.get(),bufLen);
            if (
                strncmp(buf.get(),"//@",3)==0)
            {
                copy=!copy;
            }
            else
            {
                if (
                    copy)
                {
                    res<<buf.get()<<std::endl;
                }
            }
        }
        return res.str();
    }
    
    void setSourceCode(
        const CGprogram aProgram)
    {
        mSourceCode=extractFunctions(cgGetProgramString(aProgram,CG_PROGRAM_SOURCE));
    }

    void mergeProgramParameters()
    {
        // -- Input parameters --
        mProgramParameterNameMap.clear();
        BOOST_FOREACH(
            Traits<Pass>::Ptr pass,
            mPasses)
        {
            BOOST_FOREACH(
                Traits<Parameter>::Ptr param,
                pass->mInputs)
            {
                std::pair<ParameterNameMap::iterator,bool> result=mProgramParameterNameMap.insert(std::make_pair(param->mName,param));
                if (
                    !result.second)
                {
                    Parameter &oldParam=*result.first->second;
                    RTFACT_USER_ERROR_IF_F(!Parameter::isUnitable(oldParam,*param),"Parameter mismatch in semantic %1%",Shader::semantic2str(oldParam.mSemantic));
                }
            }
        }
        //ToDo: solve for different state on different sampler parameters of same semantic
        
        // -- Varying inputs --
        mProgramVaryingNameMap.clear();
        BOOST_FOREACH(
            Traits<Pass>::Ptr pass,
            mPasses)
        {
            BOOST_FOREACH(
                Traits<Varying>::Ptr param,
                pass->mVaryings)
            {
                std::pair<VaryingNameMap::iterator,bool> result=mProgramVaryingNameMap.insert(std::make_pair(param->mName,param));
                if (
                    !result.second)
                {
                    Varying &oldParam=*result.first->second;
                    RTFACT_USER_ERROR_IF_F(!Varying::isUnitable(oldParam,*param),"Varying parameter mismatch: %1% vs. %2%",param->mName%oldParam.mName);
                }
            }
        }
    }

    void addEffectParameter(
        EffectParameterMap &aMap,
        Parameter &aParam)
    {
        std::pair<EffectParameterMap::iterator,bool> result=
            aMap.insert(std::make_pair(aParam.mSemantic,EffectParameter()));
        if (
            result.second)
        {
            result.first->second.mEffectParameter=new Parameter(NULL,aParam);
        }
        else
        {
            RTFACT_USER_ERROR_IF_F(!Parameter::isConnectable(*result.first->second.mEffectParameter,aParam),"A parameter of semantic %1% cannot be connected",Shader::semantic2str(aParam.mSemantic));
        }
        result.first->second.mConnectedParameters.push_back(&aParam);
    }

    void createEffectParameters()
    {
        // -- Input parameters --
        mEffectParameterMap.clear();
        mEffectTexParameterMap.clear();
        BOOST_FOREACH(
            ParameterNameMap::const_reference progParam1,
            mProgramParameterNameMap)
        {
            Parameter &progParam=*progParam1.second;
            if (
                progParam.isTexture())
            {
                addEffectParameter(mEffectTexParameterMap,progParam);
            }
            else
            {
                addEffectParameter(mEffectParameterMap,progParam);
            }
        }
    }

    void createMissingTextureEffectParameters()
    {
        // -- Create list of all outputs --
        std::set<Shader::Semantic> outputs;
        BOOST_FOREACH(
            Traits<Pass>::Ptr pass,
            mPasses)
        {
            outputs.insert(pass->mOutput);
        }
        
        // -- Find textures forced by code/output --
        BOOST_FOREACH(
            EffectParameterMap::const_reference param1,
            mEffectParameterMap)
        {
            const Parameter &param=*param1.second.mEffectParameter;
            if (
                mEffectTexParameterMap.find(param.mSemantic)==mEffectTexParameterMap.end() && (
                    param.mSemantic==Shader::PARAM_FBCOLOR ||
                    param.mSemantic==Shader::PARAM_FBDISTANCE ||
                    param.mSemantic==Shader::PARAM_FBNORMAL ||
                    param.mSemantic==Shader::PARAM_FBAMBIENTOCCLUSION ||
                    param.mSemantic==Shader::PARAM_FBCOLORBLEEDING ||
                    param.mForceTexture ||
                    outputs.find(param.mSemantic)!=outputs.end()))
            {
                mEffectTexParameterMap.insert(std::make_pair(param.mSemantic,EffectParameter(Parameter::createTextureParameter(param))));
            }
        }
    }
    
    void createMissingTexturePassParameters()
    {
        // -- Add missing texture parameters to passes --
        //ToDo: triple-check this, I'm not 100% sure it's correct - a place to look first if it doesn't work...
        BOOST_FOREACH(
            Traits<Pass>::Ptr pass,
            mPasses)
        {
            typedef std::map<Shader::Semantic,bool> TexMap;
            TexMap texMap;
            
            BOOST_FOREACH(
                Traits<Parameter>::Ptr param,
                pass->mInputs)
            {
                if (
                    param->isTexture())
                {
                    texMap[param->mSemantic]=true;  //overwrites if it's already there
                }
                else
                {
                    texMap.insert(std::make_pair(param->mSemantic,false));  //does not overwrite if it's already there
                }
            }
            
            BOOST_FOREACH(
                TexMap::const_reference par,
                texMap)
            {
                if (
                    !par.second)
                {
                    EffectParameterMap::const_iterator itTexture=mEffectTexParameterMap.find(par.first);
                    if (
                        itTexture!=mEffectTexParameterMap.end())
                    {
                        pass->addInputCopy(*itTexture->second.mEffectParameter);
                    }
                }
            }
            
            pass->computeSilentReads();
        }
    }

    void nameEffectParameters(
        EffectParameterMap &aMap,
        size_t &aIdxNextParam)
    {
        boost::format namer("%1%%2%");
        BOOST_FOREACH(
            EffectParameterMap::reference paramEntry,
            aMap)
        {
            if (
                paramEntry.second.mConnectedParameters.size()==1)
            {
                paramEntry.second.mEffectParameter->mName=paramEntry.second.mConnectedParameters.front()->mName;
                paramEntry.second.mConnectedParameters.clear();
            }
            else
            {
                paramEntry.second.mEffectParameter->mName=(namer%Shader::EFFECT_PARAMETER_NAME%aIdxNextParam++).str();
            }
        }
    }

    void nameEffectParameters()
    {
        size_t idxParam=0;
        nameEffectParameters(mEffectTexParameterMap,idxParam);
        nameEffectParameters(mEffectParameterMap,idxParam);
    }

    void generateParameterCgCode(
        std::ostream &aOut,
        const EffectParameterMap &aMap) const
    {
        BOOST_FOREACH(
            EffectParameterMap::const_reference paramEntry,
            aMap)
        {
            paramEntry.second.mEffectParameter->generateCgCode(aOut);
            BOOST_FOREACH(
                Traits<Parameter>::Ptr param,
                paramEntry.second.mConnectedParameters)
            {
                param->generateCgCode(aOut);
            }
        }
    }

    void generateParameterCgCode(
        std::ostream &aOut) const
    {
        aOut<<"varying float4 "<<Shader::EFFECT_PARAMETER_TEX0_NAME<<":TEXCOORD0;"<<std::endl;
        BOOST_FOREACH(
            VaryingNameMap::const_reference entry,
            mProgramVaryingNameMap)
        {
            entry.second->generateCgCode(aOut);
        }
        generateParameterCgCode(aOut,mEffectTexParameterMap);
        generateParameterCgCode(aOut,mEffectParameterMap);
    }

    template <
        bool taTestTexture>
    void wireCgEffectParameters(
        EffectParameterMap &aMap)
    {
        BOOST_FOREACH(
            EffectParameterMap::reference paramEntry,
            aMap)
        {
            paramEntry.second.mEffectParameter->updateCgParameter<true>(mCgEffect);
            const bool connect=(/*paramEntry.second.mConnectedParameters.size()>1 &&*/ (
                !taTestTexture ||
                mEffectTexParameterMap.find(paramEntry.first)==mEffectTexParameterMap.end()));
            BOOST_FOREACH(
                Traits<Parameter>::Ptr connectedParam,
                paramEntry.second.mConnectedParameters)
            {
                if (
                    connect)
                {
                    cgConnectParameter(paramEntry.second.mEffectParameter->mCgParameter,connectedParam->mCgParameter);
                    assert(cgGetError()==CG_NO_ERROR);
                }
            }
        }
    }
    
    void wireCg()
    {
        mCgTechnique=cgGetFirstTechnique(mCgEffect);
        if (
            !cgValidateTechnique(mCgTechnique))
        {
            std::cerr<<cgGetLastListing(mCgContext)<<std::endl;
            RTFACT_PROGRAM_ERROR("Invalid technique was created");
        }
        
        wireCgEffectParameters<false>(mEffectTexParameterMap);
        wireCgEffectParameters<true>(mEffectParameterMap);
        
        size_t idxPass=0;
        BOOST_FOREACH(
            Traits<Pass>::Ptr pass,
            mPasses)
        {
            pass->updateCgPass(mCgTechnique,idxPass++);
            BOOST_FOREACH(
                Traits<Parameter>::Ptr param,
                pass->mInputs)
            {
                param->updateCgParameter<false>(mCgEffect);
            }
        }
    }

public:
    std::vector<Traits<Pass>::Ptr> mPasses;
        //Postprocessor::parseEffectWiring,Postprocessor::createMasterWiring
    std::vector<size_t> mPassIdx;
        //Handler::storePassIndices

    explicit Effect(
        const CGcontext aCgContext,
        const CGeffect aCgEffect=NULL)
    :
        mCgEffect(aCgEffect),
        mCgContext(aCgContext)
    {}

    void addNewPass(
        CGpass aPass)
    {
        Pass *pass=new Pass(*this,mPasses.size());
        pass->copyFrom(aPass);
        mPasses.push_back(pass);
        if (
            mSourceCode.empty())
        {
            setSourceCode(pass->mCgProgram);
        }
    }

    Pass& addNewPass(const Pass &aPass) {
        Pass *pass=new Pass(*this,mPasses.size());
        pass->mirror(aPass);
        mPasses.push_back(pass);
        return *pass;
    }

    void forgetSelfContainedCg()
    {
        mCgEffect=NULL;
        BOOST_FOREACH(
            Traits<Pass>::Ptr pass,
            mPasses)
        {
            pass->forgetSelfContainedCg();
        }
    }

    void createCg()
    {
        std::ostringstream source;
        using std::endl;
        
        source<<"#define outputs(maSemantic)"<<endl;
        
        std::set<Traits<Effect>::Ptr> mirroredEffects;
        BOOST_FOREACH(
            Traits<Pass>::Ptr pass,
            mPasses)
        {
            BOOST_FOREACH(
                const Pass *mirroredPass,
                pass->mMirroredPasses)
            {
                mirroredEffects.insert(&mirroredPass->mEffect);
            }
        }
        
        BOOST_FOREACH(
            Traits<Effect>::Ptr effect,
            mirroredEffects)
        {
            effect->mPassIdx.resize(mPasses.size());
            std::fill(effect->mPassIdx.begin(),effect->mPassIdx.end(),size_t(-1));
        }
        
        mergeProgramParameters();
        createEffectParameters();
        createMissingTextureEffectParameters();
        nameEffectParameters();
        createMissingTexturePassParameters();
        generateParameterCgCode(source);
        
        BOOST_FOREACH(
            Traits<Effect>::Ptr effect,
            mirroredEffects)
        {
            source<<effect->mSourceCode<<endl;
        }
        
        size_t idxNextMain=0;
        boost::format mainName("%1%%2%");
        BOOST_FOREACH(
            Traits<Pass>::Ptr pass,
            mPasses)
        {
            assert(!pass->mMirroredPasses.empty());
            pass->mProgramEntryPoint=(mainName%Shader::SHADER_MAIN_NAME%idxNextMain++).str();
            source<<"float4 "<<pass->mProgramEntryPoint<<"():COLOR {"<<endl;

            BOOST_FOREACH(
                Shader::Semantic sem,
                pass->mSilentReads)
            {
                EffectParameterMap::const_reference effectParamEntry=*mEffectParameterMap.find(sem);
                source<<effectParamEntry.second.mEffectParameter->mName<<"=";
                BOOST_FOREACH(
                    Traits<Parameter>::Ptr param,
                    effectParamEntry.second.mConnectedParameters)
                {
                    source<<param->mName<<"=";
                }
                source<<"texRECT("<<mEffectTexParameterMap[sem].mEffectParameter->mName<<","<<Shader::EFFECT_PARAMETER_TEX0_NAME<<".xy);"<<endl;
            }

            pass->generateMainBodyCgCode(source);

            source<<"}"<<endl;
        }
        
        source<<"technique Main {"<<endl;
        size_t idxPass=0;
        BOOST_FOREACH(
            Traits<Pass>::Ptr pass,
            mPasses)
        {
            source<<"pass "<<Shader::EFFECT_PASS_NAME<<idxPass<<" {"<<endl;
            source<<"FragmentProgram = compile fp40 "<<pass->mProgramEntryPoint<<"();"<<endl;
            pass->generateStateAssignmentCgCode(source);
            source<<"}"<<endl;
            ++idxPass;
        }
        source<<"}"<<endl;
        
        mSourceCode=source.str();
        #ifdef RTFACT_DEBUG
        {
            std::ofstream fil("generated_effect.cgfx");
            fil<<mSourceCode;
            fil.close();
        }
        #endif

        mCgEffect=cgCreateEffect(mCgContext,mSourceCode.c_str(),NULL);
        RTFACT_PROGRAM_ERROR_IF(mCgEffect==NULL,"Invalid effect file was created");
        wireCg();
    }

    void indexPasses()
    {
        BOOST_FOREACH(
            Traits<Pass>::Ptr pass,
            mPasses)
        {
            BOOST_FOREACH(
                const Pass *mirroredPass,
                pass->mMirroredPasses)
            {
                mirroredPass->mEffect.mPassIdx[pass->mPassIdx]=mirroredPass->mPassIdx;
            }
        }
    }

    void storeParameterHandles(
        CGparameter * const aHandles)
    {
        BOOST_FOREACH(
            EffectParameterMap::const_reference entry,
            mEffectParameterMap)
        {
            aHandles[entry.first]=entry.second.mEffectParameter->mCgParameter;
        }
        BOOST_FOREACH(
            EffectParameterMap::const_reference entry,
            mEffectTexParameterMap
        ) {
            aHandles[entry.first]=entry.second.mEffectParameter->mCgParameter;   //intentionally overwrite duplicities
        }
    }
    
    void saveXML(
        std::ostream &aOut) const
    {
        using std::endl;
        aOut<<"<Effect>"<<endl;

        if (mCgContext!=NULL) aOut<<"<CgContext />"<<endl;
        if (mCgEffect!=NULL) aOut<<"<CgEffect />"<<endl;
        if (mCgTechnique!=NULL) aOut<<"<CgTechnique />"<<endl;
        //aOut<<"<SourceCode>"<<endl<<mSourceCode<<"</SourceCode>"<<endl;
        if (!mSourceCode.empty()) aOut<<"<SourceCode />"<<endl;

        if (
            !mPasses.empty())
        {
            aOut<<"<Passes>"<<endl;
            BOOST_FOREACH(
                Traits<Pass>::Ptr pass,
                mPasses)
            {
                pass->saveXML(aOut);
            }
            aOut<<"</Passes>"<<endl;
        }

        if (
            !mPassIdx.empty())
        {
            aOut<<"<PassIdx>"<<endl;
            BOOST_FOREACH(
                size_t idx,
                mPassIdx)
            {
                aOut<<"<Index val='"<<idx<<"' />"<<endl;
            }
            aOut<<"</PassIdx>"<<endl;
        }

        aOut<<"<ProgramParameters>"<<endl;
        BOOST_FOREACH(
            ParameterNameMap::const_reference entry,
            mProgramParameterNameMap)
        {
            entry.second->saveXML(aOut);
        }
        aOut<<"</ProgramParameters>"<<endl;

        aOut<<"<EffectParameters>"<<endl;
        BOOST_FOREACH(
            EffectParameterMap::const_reference entry,
            mEffectParameterMap)
        {
            aOut<<"<EffectParameterEntry>"<<endl;
            entry.second.mEffectParameter->saveXML(aOut);
            BOOST_FOREACH(
                Traits<Parameter>::Ptr param,
                entry.second.mConnectedParameters)
            {
                param->saveXML(aOut);
            }
            aOut<<"</EffectParameterEntry>"<<endl;
        }
        aOut<<"</EffectParameters>"<<endl;

        aOut<<"<EffectTexParameters>"<<endl;
        BOOST_FOREACH(
            EffectParameterMap::const_reference entry,
            mEffectTexParameterMap)
        {
            aOut<<"<EffectParameterEntry>"<<endl;
            entry.second.mEffectParameter->saveXML(aOut);
            BOOST_FOREACH(
                Traits<Parameter>::Ptr param,
                entry.second.mConnectedParameters)
            {
                param->saveXML(aOut);
            }
            aOut<<"</EffectParameterEntry>"<<endl;
        }
        aOut<<"</EffectTexParameters>"<<endl;

        aOut<<"</Effect>"<<endl;
    }
};

} // namespace Wiring

RTFACT_NAMESPACE_END

#endif // RTFACT__POSTPROCESSOR_WIRING_HPP
