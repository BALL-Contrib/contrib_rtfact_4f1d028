/* 
 *  MultiCgFXPostprocessor.hpp
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
 
 #ifndef RTFACT__MULTICGFXPOSTPROCESSOR_HPP
#define RTFACT__MULTICGFXPOSTPROCESSOR_HPP

#include <RTfact/Config/Common.hpp>

#include <boost/type_traits/is_base_of.hpp>
 
#include <boost/mpl/assert.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/transform.hpp>
namespace mpl_ {
    using namespace boost::mpl::placeholders;
}

#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/algorithm/transformation/zip.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/fusion/support.hpp>
#include <boost/fusion/tuple.hpp>

#include <RTfact/Concept/Postprocessor.hpp>

#include <RTfact/Model/Postprocessor/Detail/CgEffectHandler.hpp>
#include <RTfact/Model/Postprocessor/Detail/Metafunctions.hpp>
#include <RTfact/Model/Postprocessor/Detail/Wiring.hpp>

#include <RTfact/Utils/Shaders/Includes.hpp>

RTFACT_NAMESPACE_BEGIN

template <typename tPostprocessor>
struct setEnabledEffects_enableHandler
{
    tPostprocessor &mThis;
    setEnabledEffects_enableHandler(tPostprocessor *aThis) : mThis(*aThis) {}

    typedef size_t result_type;

    template <
        typename tPair>
    result_type operator() (
        const result_type aIdx,
        const tPair &aPair) const
    {
        tPair &pair=const_cast<tPair&>(aPair); //aPair can't be non-const directly because of ICC (25.10.2010)
        if (
            mThis.mActiveWiringConfig.mEnabledEffects[aIdx])
        {
            boost::fusion::get<0>(pair)=boost::fusion::get<1>(aPair);
        }
        else
        {
            boost::fusion::get<0>(pair)=NULL;
        }
        return aIdx+1;
    }
};

/*
 * {get|set}Parameter_viaHandler are templated by index into effect list and the list's size.
 * For taIdx<taSize, the appropriate handler is accessed.
 * For taIdx>=taSize, a template specialization is created which does nothing.
 * This approach is necessary to prevent the compiler from generating erroneous code
 * (e.g. trying to access the 5th component of a 4-vector).
 */

template <
    typename tSupportedEffectHandlers,
    size_t taIdx,
    size_t taSize>
struct getParameter_viaHandler
{
    template <
        typename tParameterValue>
    static const bool call(
        const tSupportedEffectHandlers &aHandlers,
        const Shader::Parameters &aParameters,
        const Shader::Semantic aParameter,
        tParameterValue &aValue)
    {
        return boost::fusion::at<boost::mpl::int_<taIdx> >(aHandlers)->mEffect.getParameter(aParameter,aValue);
    }

    template <
        typename tParameterValue>
    static const bool callMulti(
        const tSupportedEffectHandlers &aHandlers,
        const Shader::Parameters &aParameters,
        const Shader::Semantic aParameter,
        tParameterValue *aValue)
    {
        return boost::fusion::at<boost::mpl::int_<taIdx> >(aHandlers)->mEffect.getMultiParameter(aParameter,aValue);
    }
};

#define IDX_SPECIALIZE(maZ,maIdx,maSize) \
  template < \
    typename tSupportedEffectHandlers> \
    struct getParameter_viaHandler<tSupportedEffectHandlers, BOOST_PP_ADD(maIdx,maSize),maSize> \
    { \
        template < \
            typename tParameterValue> \
        static const bool call( \
            const tSupportedEffectHandlers &aHandlers, \
            const Shader::Parameters &aParameters, \
            const Shader::Semantic aParameter, \
            tParameterValue &aValue) \
        { \
            return false; \
        } \
        \
        template < \
            typename tParameterValue> \
        static const bool callMulti( \
            const tSupportedEffectHandlers &aHandlers, \
            const Shader::Parameters &aParameters, \
            const Shader::Semantic aParameter, \
            tParameterValue *aValue) \
        { \
            return false; \
        } \
    };

#define SIZE_SPECIALIZE(maZ,maSize,maUnused) \
    BOOST_PP_REPEAT_ ## maZ (BOOST_PP_SUB(FUSION_MAX_VECTOR_SIZE,maSize),IDX_SPECIALIZE,maSize)

BOOST_PP_REPEAT(FUSION_MAX_VECTOR_SIZE,SIZE_SPECIALIZE,%%)

#undef IDX_SPECIALIZE
#undef SIZE_SPECIALIZE

template <
    typename tSupportedEffectHandlers,
    size_t taIdx,
    size_t taSize>
struct setParameter_viaHandler
{
    template <
        typename tParameterValue>
    static const bool set(
        const tSupportedEffectHandlers &aHandlers,
        const Shader::Parameters &aParameters,
        const Shader::Semantic aParameter,
        const tParameterValue &aValue)
    {
        return boost::fusion::at<boost::mpl::int_<taIdx> >(aHandlers)->mEffect.setParameter(aParameter,aValue);
    }

    template <
        typename tParameterValue>
    static const bool setMulti(
        const tSupportedEffectHandlers &aHandlers,
        const Shader::Parameters &aParameters,
        const Shader::Semantic aParameter,
        const tParameterValue *aValue)
    {
        return boost::fusion::at<boost::mpl::int_<taIdx> >(aHandlers)->mEffect.setMultiParameter(aParameter,aValue);
    }

    template <
        typename tParameterValue>
    static const bool update(
        const tSupportedEffectHandlers &aHandlers,
        const Shader::Parameters &aParameters,
        const Shader::Semantic aParameter,
        const tParameterValue &aValue)
    {
        return boost::fusion::at<boost::mpl::int_<taIdx> >(aHandlers)->mEffect.updateParameter(aParameters,aParameter);
    }
};

#define IDX_SPECIALIZE(maZ,maIdx,maSize) \
    template <typename tSupportedEffectHandlers> \
    struct setParameter_viaHandler<tSupportedEffectHandlers, BOOST_PP_ADD(maIdx,maSize),maSize> \
    { \
        template < \
            typename tParameterValue> \
        static const bool set( \
            const tSupportedEffectHandlers &aHandlers, \
            const Shader::Parameters &aParameters, \
            const Shader::Semantic aParameter, \
            const tParameterValue &aValue) \
        { \
            return false; \
        } \
        \
        template < \
            typename tParameterValue> \
        static const bool setMulti( \
            const tSupportedEffectHandlers &aHandlers, \
            const Shader::Parameters &aParameters, \
            const Shader::Semantic aParameter, \
            const tParameterValue *aValue) \
        { \
            return false; \
        } \
        \
        template < \
            typename tParameterValue> \
        static const bool update( \
            const tSupportedEffectHandlers &aHandlers, \
            const Shader::Parameters &aParameters, \
            const Shader::Semantic aParameter, \
            const tParameterValue &aValue) \
        { \
            return false; \
        } \
    };

#define SIZE_SPECIALIZE(maZ,maSize,maUnused) \
    BOOST_PP_REPEAT_ ## maZ (BOOST_PP_SUB(FUSION_MAX_VECTOR_SIZE,maSize),IDX_SPECIALIZE,maSize)

BOOST_PP_REPEAT(FUSION_MAX_VECTOR_SIZE,SIZE_SPECIALIZE,%%)

#undef IDX_SPECIALIZE
#undef SIZE_SPECIALIZE


/*!
 * MultiCgFXPostprocessor is the master class of CgFX postprocessing in RTfact.
 * It creates and stores effect runtime objects and manages CgFX.
 * tSupportedEffects must be a boost::fusion Random Access Sequence of effect classes (models of ShaderEffect)
 * in the order they are to be applied during postprocessing.
 * The four boolean ta* parameters are to be left at their default values.
 */
template <
    typename tSupportedEffects,
    bool taPreRender=boost::mpl::fold<tSupportedEffects,boost::mpl::bool_<false>,orPreRender<mpl_::_2,mpl_::_1> >::type::value,
    bool taPrePass=boost::mpl::fold<tSupportedEffects,boost::mpl::bool_<false>,orPrePass<mpl_::_2,mpl_::_1> >::type::value,
    bool taPostPass=boost::mpl::fold<tSupportedEffects,boost::mpl::bool_<false>,orPostPass<mpl_::_2,mpl_::_1> >::type::value,
    bool taPostRender=boost::mpl::fold<tSupportedEffects,boost::mpl::bool_<false>,orPostRender<mpl_::_2,mpl_::_1> >::type::value>
class MultiCgFXPostprocessor :
    public Postprocessor
{
    RTFACT_STATIC_ASSERT((boost::fusion::traits::is_sequence<tSupportedEffects>::value));   //it's a fusion sequence
    RTFACT_STATIC_ASSERT((boost::is_base_of<boost::fusion::random_access_traversal_tag,typename boost::fusion::traits::category_of<tSupportedEffects>::type>::value));  //it's a random access sequence

public:
    typedef MultiCgFXPostprocessor Type;

private:

    friend  struct setEnabledEffects_enableHandler<Type>;

    template <
        typename tType>
    struct makeHandlerPtr
    {
        typedef typename Traits<CgEffectHandler<tType> >::Ptr type;
    };

    typedef typename boost::mpl::transform<tSupportedEffects,makeHandlerPtr<mpl_::_1> >::type SupportedEffectHandlers;
    
    /*
     * Two instances of this struct are stored: mActiveWiringConfig and mBuiltWiringConfig. These serve as a cache/buffer for state changes.
     * Active stores the currently setup configuration, i.e. it's the one that gets changed with every oustide call.
     * Built stores the configuration for which wiring was last created. Creating wiring is quite a costly operation,
     * so it makes sense to cache changes and only rebuild wiring when it's required (for rendering).
     * This is done by always storing the last built wiring in mBuiltWiring.
     * The wiring used for rendering is stored in mMasterWiring. This is either a pointer to mBuiltWiring, or NULL (iff there were
     * any configration changes since last building).
     * When rendering is requested, mMasterWiring is NULL and ActiveConfig!=BuiltConfig, wiring is rebuilt.
     * See also validateRenderingSupport()
     */
    struct WiringConfig
    {
        std::vector<bool> mEnabledEffects;
        bool mOutputRequested[Shader::PARAM_COUNT];
        bool mColorOutputRequested;
        
        WiringConfig() :
            mColorOutputRequested(false)
        {
            std::fill_n(mOutputRequested,size_t(Shader::PARAM_COUNT),false);
            mOutputRequested[Shader::PARAM_FBCOLOR]=true;
        }
        
        const bool operator==(
            const WiringConfig &aArg) const
        {
            return (
                mColorOutputRequested==aArg.mColorOutputRequested &&
                mEnabledEffects==aArg.mEnabledEffects &&
                std::equal(mOutputRequested,mOutputRequested+Shader::PARAM_COUNT,aArg.mOutputRequested));
        }
        
        const bool operator!=(
            const WiringConfig &aArg) const
        {
            return !(*this==aArg);
        }
    };

    SupportedEffectHandlers mEffectHandlers;
    SupportedEffectHandlers mEnabledHandlers;   //has NULLs in place of disabled handlers
    CGcontext mCgContext;
    std::vector<Traits<Wiring::Effect>::Ptr> mEffectWirings,mEnabledEffectWirings;
    std::vector<Traits<Wiring::Pass>::Ptr> mAllPasses,mEnabledEffectPasses;
    Traits<Wiring::Effect>::Ptr mMasterWiring,mBuiltWiring;
    mutable Shader::Parameters mParameters;
    SupportedEffectHandlers mPreRenderEffects,mPostRenderEffects;
    bool mHasPreRender,mHasPostRender;
    std::vector<SupportedEffectHandlers> mPrePassEffects,mPostPassEffects;
    std::vector<bool> mHasPrePass,mHasPostPass;
    std::vector<GLbitfield> mPrePassClear;
    typedef std::pair<Shader::Semantic,CGparameter> TextureSettie;
    std::vector<std::vector<TextureSettie> > mPassTextures;
    WiringConfig mActiveWiringConfig,mBuiltWiringConfig;
    size_t mActiveParameterManager[Shader::PARAM_COUNT];
    std::list<size_t> mParameterManagers[Shader::PARAM_COUNT];
    typedef std::set<Shader::Semantic> SupportedParameters;
    SupportedParameters mSupportedParameters;
    GLuint mOutputGLFramebuffer;
    GLfloat mMatrixWorldToEye[9];
    IO::PathList mShaderPaths;
    
//--------------------------------------------------------------------------------------------------
//
// Callable structures (for use by boost::mpl, boost::fusion and std) with template methods.
// These cannot be declared in functions, so they are gathered here.
// Each name is prefixed by the method which uses the structure.
//
//--------------------------------------------------------------------------------------------------

    struct init_initHandler
    {
        MultiCgFXPostprocessor &mThis;
        init_initHandler(MultiCgFXPostprocessor *aThis) : mThis(*aThis) {}
        
        template <
            typename tHandlerPtr>
        void operator() (
            tHandlerPtr &aHandler) const
        {
            aHandler=new typename Traits<tHandlerPtr>::Base(mThis.mCgContext);
            aHandler->init(mThis.getShaderPathList());
            mThis.mEffectWirings.push_back(aHandler->mWiring);
        }
    };
    
    struct storeParameterManagers_index
    {
        MultiCgFXPostprocessor &mThis;
        storeParameterManagers_index(MultiCgFXPostprocessor *aThis) : mThis(*aThis) {}

        typedef size_t result_type;
        
        template <
            typename tHandlerPtr>
        result_type operator() (
            const result_type aIdx,
            tHandlerPtr &aHandler) const
        {
            BOOST_FOREACH(
                Shader::Semantic par,
                aHandler->mEffect.getSettableParameters())
            {
                mThis.mParameterManagers[par].push_back(aIdx);
                mThis.mSupportedParameters.insert(par);
            }
            return aIdx+1;
        }
    };
    
    struct parseEnabledEffectWiring_addWiring
    {
        MultiCgFXPostprocessor &mThis;
        parseEnabledEffectWiring_addWiring(MultiCgFXPostprocessor *aThis) : mThis(*aThis) {}
        
        template <
            typename tHandlerPtr>
        void operator() (
            tHandlerPtr &aHandler) const
        {
            if (
                aHandler!=NULL)
            {
                mThis.mEnabledEffectWirings.push_back(aHandler->mWiring);
            }
        }
    };
    
    struct indexPasses_storePassIndices
    {
        template <
            typename tHandlerPtr>
        void operator() (
            const tHandlerPtr aHandler) const
        {
            aHandler->storePassIndices();
        }
    };
    
    struct createRenderEventLists_enablePreRender
    {
        typedef size_t result_type;
        
        template <
            typename tHandlerPtr>
        result_type operator() (
            const result_type aState,
            tHandlerPtr &aHandler) const
        {
            if (
                aHandler==NULL ||
                !Traits<tHandlerPtr>::Base::Effect::mHasPreRender)
            {
                aHandler=NULL;
                return aState;
            }
            else
            {
                return aState+1;
            }
        }
    };
    
    struct createRenderEventLists_enablePrePass
    {
        const size_t mIdxPass;
        createRenderEventLists_enablePrePass(const size_t aIdxPass) : mIdxPass(aIdxPass) {}

        typedef size_t result_type;
        
        template <
            typename tHandlerPtr>
        result_type operator() (
            const result_type aState,
            tHandlerPtr &aHandler) const
        {
            if (
                aHandler==NULL ||
                aHandler->mPassId[mIdxPass]==-1 ||
                (Traits<tHandlerPtr>::Base::Effect::mHasPrePass & (1<<aHandler->mPassId[mIdxPass]))==0)
            {
                aHandler=NULL;
                return aState;
            }
            else
            {
                return aState+1;
            }
        }
    };
    
    struct createRenderEventLists_enablePostPass
    {
        const size_t mIdxPass;
        createRenderEventLists_enablePostPass(const size_t aIdxPass) : mIdxPass(aIdxPass) {}

        typedef size_t result_type;
        
        template <
            typename tHandlerPtr>
        result_type operator() (
            const result_type aState,
            tHandlerPtr &aHandler) const
        {
            if (
                aHandler==NULL ||
                aHandler->mPassId[mIdxPass]==-1 ||
                (Traits<tHandlerPtr>::Base::Effect::mHasPostPass & (1<<aHandler->mPassId[mIdxPass]))==0)
            {
                aHandler=NULL;
                return aState;
            }
            else
            {
                return aState+1;
            }
        }
    };
    
    struct createRenderEventLists_enablePostRender
    {
        typedef size_t result_type;
        
        template <
            typename tHandlerPtr>
        result_type operator() (
            const result_type aState,
            tHandlerPtr &aHandler) const
        {
            if (
                aHandler==NULL ||
                !Traits<tHandlerPtr>::Base::Effect::mHasPostRender)
            {
                aHandler=NULL;
                return aState;
            }
            else
            {
                return aState+1;
            }
        }
    };
    
    struct createRenderEventLists_setPrePassClear
    {
        MultiCgFXPostprocessor &mThis;
        const size_t mIdxPass;
        createRenderEventLists_setPrePassClear(MultiCgFXPostprocessor *aThis,const size_t aIdxPass) : mThis(*aThis),mIdxPass(aIdxPass) {}

        template <
            typename tHandlerPtr>
        void operator() (
            const tHandlerPtr aHandler) const
        {
            if (
                aHandler!=NULL)
            {
                typedef typename Traits<tHandlerPtr>::Base::Effect Effect;
                size_t passId=aHandler->mPassId[mIdxPass];
                if (
                    passId!=-1 &&
                    (Effect::mPrePassClearColor & (1<<passId))!=0)
                {
                    mThis.mPrePassClear[mIdxPass] |= GL_COLOR_BUFFER_BIT;
                }
            }
        }
    };
    
    struct setParameters_set
    {
        MultiCgFXPostprocessor &mThis;
        setParameters_set(MultiCgFXPostprocessor *aThis) : mThis(*aThis) {}

        template <
            typename tHandlerPtr>
        void operator() (
            const tHandlerPtr aHandler) const
        {
            if (
                aHandler!=NULL)
            {
                aHandler->mEffect.updateParameters(mThis.mParameters);
            }
        }
    };
    
    struct render_preRender
    {
        const MultiCgFXPostprocessor &mThis;
        render_preRender(const MultiCgFXPostprocessor *aThis) : mThis(*aThis) {}

        template <
            typename tHandlerPtr>
        void operator() (
            const tHandlerPtr aHandler) const
        {
            if (
                aHandler!=NULL)
            {
                aHandler->mEffect.preRender(mThis.mParameters);
            }
        }
    };
    
    struct render_prePass
    {
        const MultiCgFXPostprocessor &mThis;
        const size_t mIdxPass;
        render_prePass(const MultiCgFXPostprocessor *aThis,const size_t aIdxPass) : mThis(*aThis),mIdxPass(aIdxPass) {}

        template <
            typename tHandlerPtr>
        void operator() (
            const tHandlerPtr aHandler) const
        {
            if (
                aHandler!=NULL)
            {
                aHandler->mEffect.prePass(aHandler->mPassId[mIdxPass],mThis.mParameters);
            }
        }
    };

    struct render_postPass
    {
        const MultiCgFXPostprocessor &mThis;
        const size_t mIdxPass;
        render_postPass(const MultiCgFXPostprocessor *aThis,const size_t aIdxPass) : mThis(*aThis),mIdxPass(aIdxPass) {}

        template <
            typename tHandlerPtr>
        void operator() (
            const tHandlerPtr aHandler) const
        {
            if (
                aHandler!=NULL)
            {
                aHandler->mEffect.postPass(aHandler->mPassId[mIdxPass],mThis.mParameters);
            }
        }
    };
    
    struct render_postRender
    {
        const MultiCgFXPostprocessor &mThis;
        render_postRender(const MultiCgFXPostprocessor *aThis) : mThis(*aThis) {}

        template <
            typename tHandlerPtr>
        void operator() (
            const tHandlerPtr aHandler) const
        {
            if (
                aHandler!=NULL)
            {
                aHandler->mEffect.postRender(mThis.mParameters);
            }
        }
    };
    
    struct recomputeParameterManagers_index
    {
        MultiCgFXPostprocessor &mThis;
        recomputeParameterManagers_index(MultiCgFXPostprocessor *aThis) : mThis(*aThis) {}

        typedef size_t result_type;
        
        template <
            typename tHandlerPtr>
        result_type operator() (
            const result_type aIdx,
            tHandlerPtr &aHandler) const
        {
            if (
                aHandler!=NULL)
            {
                BOOST_FOREACH(
                    Shader::Semantic par,
                    aHandler->mEffect.getSettableParameters())
                {
                    if (
                        mThis.mActiveParameterManager[par]==-1)
                    {
                        mThis.mActiveParameterManager[par]=aIdx;
                    }
                }
            }
            return aIdx+1;
        }
    };
    
    //nonEmpty has assertion use only
    struct storeParameterManagers_nonEmpty
    {
        const bool operator() (
            const std::list<size_t> &aList) const
        {
            return !aList.empty();
        }
    };
    
//--------------------------------------------------------------------------------------------------
//
// Private methods
//
//--------------------------------------------------------------------------------------------------

    /*!
     * Gathers up passes from all effects into mAllPasses.
     * Called just once at init.
     */
    void parseEffectWiring()
    {
        assert(mAllPasses.empty());
        BOOST_FOREACH(
            Traits<Wiring::Effect>::Ptr effect,
            mEffectWirings)
        {
            BOOST_FOREACH(
                Traits<Wiring::Pass>::Ptr pass,
                effect->mPasses)
            {
                mAllPasses.push_back(pass);
            }
        }
    }
    
    /*!
     * For each semantic, creates a list of all effects which control this semantic,
     * that is, for which this semantic is a settableParameter.
     */
    void storeParameterManagers()
    {        
        RTFACT_ASSERT(std::find_if(mParameterManagers,mParameterManagers+Shader::PARAM_COUNT,storeParameterManagers_nonEmpty())==mParameterManagers+Shader::PARAM_COUNT);
        
        boost::fusion::fold(mEffectHandlers,0,storeParameterManagers_index(this));
    }
    
    /*!
     * Recomputes the lists of enabled effects' wirings and of all passes of enabled effects.
     */
    void parseEnabledEffectWiring()
    {
        mEnabledEffectWirings.clear();
        boost::fusion::for_each(mEnabledHandlers,parseEnabledEffectWiring_addWiring(this));
        mEnabledEffectPasses.clear();
        BOOST_FOREACH(
            Traits<Wiring::Effect>::Ptr effect,
            mEnabledEffectWirings)
        {
            mEnabledEffectPasses.insert(mEnabledEffectPasses.end(),effect->mPasses.begin(),effect->mPasses.end());
        }
    }

    /*!
     * Generates OpenGL texture names for all textures necessary for correct effect execution.
     * Also stores these names into <code>mParameters</code>.
     * \param aRequired An array of length <code>Shader::PARAM_COUNT</code>, giving number of 
     *  textures required for each semantic. Valid values are 0, 1 or 2 (2 only happens if a pass
     *  reads a texture and writes to it at the same time).
     */
    void createTextures(
        const unsigned short * const aRequired)
    {
        GLuint tex[2];
        for (
            size_t param=0;param<Shader::PARAM_COUNT;++param)   //ToDo: this might be changed to a static_for
        {
            glGenTextures(aRequired[param],tex);
            switch (
                aRequired[param])
            {
                case 1:
                    mParameters.mParamTextures[param]=mParameters.mParamWriteTextures[param]=tex[0];
                    break;
                case 2:
                    mParameters.mParamTextures[param]=tex[0];
                    mParameters.mParamWriteTextures[param]=tex[1];
                    break;
            }
        }
    }
    
    void destroyTextures()
    {
        glDeleteTextures(Shader::PARAM_COUNT,mParameters.mParamTextures);
        std::fill_n(mParameters.mParamTextures,size_t(Shader::PARAM_COUNT),0);
        glDeleteTextures(Shader::PARAM_COUNT,mParameters.mParamWriteTextures);
        std::fill_n(mParameters.mParamWriteTextures,size_t(Shader::PARAM_COUNT),0);
    }

    /*!
     * Generates an OpenGL framebuffer object for each pass whose output goes into a texture.
     */
    void createFramebuffers()
    {
        std::vector<Traits<Wiring::Pass>::Ptr>::const_iterator itEnd=mMasterWiring->mPasses.end();
        for (
            std::vector<Traits<Wiring::Pass>::Ptr>::const_iterator itPass=mMasterWiring->mPasses.begin();
            itPass!=itEnd;
            ++itPass)
        {
            const Wiring::Pass &pass=**itPass;
            if (
                mParameters.mFramebuffers[pass.mOutput]==0 &&  (
                    isOutputRequested(pass.mOutput) ||
                    std::find_if(boost::next(itPass),itEnd,Wiring::Pass::Reads(pass.mOutput))!=mMasterWiring->mPasses.end()))
            {
                glGenFramebuffers/*ARB*/(1,&mParameters.mFramebuffers[pass.mOutput]);
            }
        }
    }
    
    void destroyFramebuffers() {
        glDeleteFramebuffers/*ARB*/(Shader::PARAM_COUNT,mParameters.mFramebuffers);
        std::fill_n(mParameters.mFramebuffers,size_t(Shader::PARAM_COUNT),0);
    }

    /*!
     * Forces texture to be created for aParam (if aParam is used at all).
     * This is called for data which exists in texture form (such as framebuffer buffers).
     * \param aParam Semantic to create texture for.
     * \param aTextures Array of length <code>Shader::PARAM_COUNT</code>, storing number of textures required (0, 1 or 2) for each semantic.
     * \param aParamsUsed A semantic stage map conatining all parameters actually used by the postprocessor.
     */
    void forceTexture(
        const Shader::Semantic aParam,
        unsigned short * const aTextures,
        const SemanticStageMap &aParamsUsed)
    {
        if (
            aTextures[aParam]==0 &&
            aParamsUsed.find(aParam)!=aParamsUsed.end())
        {
            aTextures[aParam]=1;
        }
    }

    /*!
     * Creates master wiring, including its CgFX effect.
     * Only to be called with no presently active master wiring.
     * Tries to use the cached wiring, if possible (configurations match). Otherwise, constructs a new one.
     */
    const bool createMasterWiring()
    {
        RTFACT_ASSERT(mMasterWiring==NULL);
        if (
            mBuiltWiring!=NULL && mActiveWiringConfig==mBuiltWiringConfig)
        {
            mMasterWiring=mBuiltWiring;
            return false;
        }
        
        typedef Traits<Wiring::Pass>::Ptr PassPtr;
        typedef Traits<Wiring::Parameter>::Ptr ParamPtr;
        
        destroyTextures();
        destroyFramebuffers();
        
        mMasterWiring=new Wiring::Effect(mCgContext);
        
        parseEnabledEffectWiring();
        
        /*
         * This map stores the current stage each parameter is in.
         * Initially, all parameters are at stage 0.
         * Each time a pass outputs a parameter, it increases its stage by 1.
         */
        SemanticStageMap semanticStageMap;
        
        // Clear stages
        BOOST_FOREACH(
            PassPtr pass,
            mEnabledEffectPasses)
        {
            BOOST_FOREACH(
                ParamPtr param,
                pass->mInputs)
            {
                semanticStageMap[param->mSemantic]=0;
            }
        }
        
        // "Simulate" pipeline progress
        /*
         * Stores the stage each parameter must reach before the pass can consume it.
         * That is, the number of writers of this parameter before this pass.
         */
        BOOST_FOREACH(
            PassPtr pass,
            mEnabledEffectPasses)
        {
            BOOST_FOREACH(
                ParamPtr param,
                pass->mInputs)
            {
                param->setStage(semanticStageMap[param->mSemantic]);
            }
            ++semanticStageMap[pass->mOutput];
        }
        
        // Clear stages again
        BOOST_FOREACH(
            SemanticStageMap::reference entry,
            semanticStageMap)
        {
            entry.second=0;
        }
        
        typedef std::list<PassPtr> PassesLeft;
        PassesLeft passesLeft(mEnabledEffectPasses.begin(),mEnabledEffectPasses.end());
        
        /*
         * Merge passes if possible.
         */
        while (
            !passesLeft.empty())
        {
            RTFACT_USER_ERROR_IF(!passesLeft.front()->isReady(semanticStageMap),"Cyclic dependencies in shader parameters");
            Wiring::Pass &newPass=mMasterWiring->addNewPass(*passesLeft.front());
            passesLeft.pop_front();
            ++semanticStageMap[newPass.mOutput];
            for (
                PassesLeft::iterator itPass=passesLeft.begin();
                itPass!=passesLeft.end();)
            {
                const Wiring::Pass &candidatePass=**itPass;
                //ToDo: the following condition could be relaxed further, up to:
                //candidatePass can be appended if no future passes read newPass.mOutput as a texture and all future passes reading newPass.mOutput can be appended too (if they exist)
                if (
                    (
                        //They output the same thing, or there is no reader (other than candidatePass) to newPass's output
                        candidatePass.mOutput==newPass.mOutput || (
                            !mActiveWiringConfig.mOutputRequested[newPass.mOutput] &&
                            std::find_if(passesLeft.begin(),itPass,Wiring::Pass::Reads(newPass.mOutput))==itPass &&
                            std::find_if(boost::next(itPass),passesLeft.end(),Wiring::Pass::Reads(newPass.mOutput))==passesLeft.end())) &&
                    !candidatePass.hasTextureInput(newPass.mOutput) &&
                    candidatePass.isReady(semanticStageMap) &&
                    Wiring::Pass::isStateCompatible(newPass,candidatePass))
                {
                    newPass.append(candidatePass);
                    ++semanticStageMap[candidatePass.mOutput];
                    itPass=passesLeft.erase(itPass);
                } else {
                    ++itPass;
                }
            }
        }
        
        mMasterWiring->createCg();
        
        unsigned short paramTexturesRequired[Shader::PARAM_COUNT];
        std::fill_n(paramTexturesRequired,size_t(Shader::PARAM_COUNT),0);
        
        BOOST_FOREACH(
            PassPtr pass,
            mMasterWiring->mPasses)
        {
            pass->storeRequiredTextures(paramTexturesRequired);
        }
        
        forceTexture(Shader::PARAM_FBCOLOR,paramTexturesRequired,semanticStageMap);
        forceTexture(Shader::PARAM_FBDISTANCE,paramTexturesRequired,semanticStageMap);
        forceTexture(Shader::PARAM_FBNORMAL,paramTexturesRequired,semanticStageMap);
		forceTexture(Shader::PARAM_FBAMBIENTOCCLUSION,paramTexturesRequired,semanticStageMap);
        forceTexture(Shader::PARAM_FBCOLORBLEEDING,paramTexturesRequired,semanticStageMap);
        
        createTextures(paramTexturesRequired);
        createFramebuffers();
        
        mBuiltWiringConfig=mActiveWiringConfig;
        mBuiltWiring=mMasterWiring;
        return true;
    }

    void indexPasses()
    {
        mMasterWiring->indexPasses();
        boost::fusion::for_each(mEffectHandlers,indexPasses_storePassIndices());
    }

    void storeParameterHandles()
    {
        std::fill_n(mParameters.mParam,size_t(Shader::PARAM_COUNT),CGparameter(NULL));
        mMasterWiring->storeParameterHandles(mParameters.mParam);
    }

    void createRenderEventLists()
    {
        if (
            taPreRender)    //static if
        {
            mPreRenderEffects=mEnabledHandlers;
            size_t res=boost::fusion::fold(
                mPreRenderEffects,
                0,
                createRenderEventLists_enablePreRender());
            mHasPreRender=(res>0);
        }
        if (
            taPrePass)  //static if
        {
            mPrePassEffects.resize(mMasterWiring->mPasses.size());
            for (
                size_t idxPass=0;idxPass<mPrePassEffects.size();++idxPass)
            {
                mPrePassEffects[idxPass]=mEnabledHandlers;
                size_t res=boost::fusion::fold(
                    mPrePassEffects[idxPass],
                    0,
                    createRenderEventLists_enablePrePass(idxPass));
                mHasPrePass[idxPass]=(res>0);
            }
        }
        if (
            taPostPass)  //static if
        {
            mPostPassEffects.resize(mMasterWiring->mPasses.size());
            for (
                size_t idxPass=0;idxPass<mPrePassEffects.size();++idxPass)
            {
                mPostPassEffects[idxPass]=mEnabledHandlers;
                size_t res=boost::fusion::fold(
                    mPostPassEffects[idxPass],
                    0,
                    createRenderEventLists_enablePostPass(idxPass));
                mHasPostPass[idxPass]=(res>0);
            }
        }
        if (
            taPostRender)    //static if
        {
            mPostRenderEffects=mEnabledHandlers;
            size_t res=boost::fusion::fold(
                mPostRenderEffects,
                0,
                createRenderEventLists_enablePostRender());
            mHasPostRender=(res>0);
        }
        
        mPrePassClear.assign(mMasterWiring->mPasses.size(),0);
        for (
            size_t idxPass=0;idxPass<mMasterWiring->mPasses.size();++idxPass)
        {
            boost::fusion::for_each(mEnabledHandlers,createRenderEventLists_setPrePassClear(this,idxPass));
        }
    }

    void createTextureSetties()
    {
        mPassTextures.clear();
        mPassTextures.resize(mMasterWiring->mPasses.size());
        for (
            size_t idxPass=0;idxPass<mMasterWiring->mPasses.size();++idxPass)
        {
            mPassTextures[idxPass].clear();
            BOOST_FOREACH(
                Traits<Wiring::Parameter>::Ptr param,
                mMasterWiring->mPasses[idxPass]->mInputs)
            {
                if (
                    param->isTexture())
                {
                    mPassTextures[idxPass].push_back(TextureSettie(param->mSemantic,param->mCgParameter));
                }
            }
        }
    }

    void setParameters()
    {
        boost::fusion::for_each(mEnabledHandlers,setParameters_set(this));
    }

    void setAutomaticParameters(
        const size_t aIdxPass,
        const uint aWindowResX,
        const uint aWindowResY) const
    {
        const CGparameter * const par=mParameters.mParam;
        if (
            par[Shader::PARAM_WINDOWRES]!=NULL)
        {
            cgSetParameter2i(par[Shader::PARAM_WINDOWRES],aWindowResX,aWindowResY);
        }
        
        BOOST_FOREACH(
            const TextureSettie &tex,
            mPassTextures[aIdxPass])
        {
            cgGLSetTextureParameter(tex.second,mParameters.mParamTextures[tex.first]);
        }
        Shader::Semantic out=mMasterWiring->mPasses[aIdxPass]->mOutput;
        GLuint framebuffer=mParameters.mFramebuffers[out];
        if (
            framebuffer==0)
        {
            framebuffer=mOutputGLFramebuffer;
        }
        glBindFramebuffer/*ARB*/(GL_DRAW_FRAMEBUFFER/*_ARB*/,framebuffer);
        if (
            framebuffer!=mOutputGLFramebuffer)
        {
            glBindTexture(GL_TEXTURE_RECTANGLE_ARB,mParameters.mParamWriteTextures[out]);
            glTexImage2D(
                GL_TEXTURE_RECTANGLE_ARB,
                0,
                GL_RGBA32F_ARB,
                aWindowResX,
                aWindowResY,
                0,
                GL_RGBA,
                GL_FLOAT,
                NULL);
            glBindTexture(GL_TEXTURE_RECTANGLE_ARB,0);
            glFramebufferTexture2D/*ARB*/(
                GL_DRAW_FRAMEBUFFER/*_ARB*/,
                GL_COLOR_ATTACHMENT0/*_ARB*/,
                GL_TEXTURE_RECTANGLE_ARB,
                mParameters.mParamWriteTextures[out],
                0); //no level
        }
    }

    void swapReadWriteTextures(
        const size_t aIdxPass) const
    {
        Shader::Semantic out=mMasterWiring->mPasses[aIdxPass]->mOutput;
        std::swap(mParameters.mParamTextures[out],mParameters.mParamWriteTextures[out]);
    }
    
    void createRenderingSupport()
    {
        if (
            createMasterWiring())
        {
            indexPasses();
            storeParameterHandles();
            createRenderEventLists();
            createTextureSetties();
            setParameters();
        }
    }
    
    void recomputeParameterManagers()
    {
        std::fill_n(mActiveParameterManager,size_t(Shader::PARAM_COUNT),-1);
        boost::fusion::fold(mEnabledHandlers,0,recomputeParameterManagers_index(this));
    }
    
    void updateCameraMatrix() const
    {
        mParameters.mEyeTransform=mMatrixWorldToEye;
        if (
            mParameters.mParam[Shader::PARAM_EYETRANSFORM]!=NULL)
        {
            cgSetMatrixParameterfc(mParameters.mParam[Shader::PARAM_EYETRANSFORM],mParameters.mEyeTransform);
        }
    }
    
    void updateFocusDistance() const
    {
        if (
            mParameters.mParam[Shader::PARAM_FOCUSDISTANCE]!=NULL)
        {
            cgSetParameter1f(mParameters.mParam[Shader::PARAM_FOCUSDISTANCE],mParameters.mFocusDistance);
        }
    }

public:

    enum {
        EFFECT_COUNT = boost::mpl::size<tSupportedEffects>::value
    };
    
    MultiCgFXPostprocessor() :
        mMasterWiring(NULL),
        mBuiltWiring(NULL),
        mOutputGLFramebuffer(0)
    {}
    
    void init(
        const std::vector<bool> &aEnabledEffects)
    {
        mCgContext=cgCreateContext();
        RTFACT_PROGRAM_ERROR_IF(mCgContext==NULL,"Error creating Cg context");
        cgGLRegisterStates(mCgContext);
        cgGLSetManageTextureParameters(mCgContext,CG_TRUE);
        cgSetSemanticCasePolicy(CG_UNCHANGED_CASE_POLICY);

        // Create and initialize all effect handlers
        // Also gathers up their wiring into mEffectWirings
        boost::fusion::for_each(mEffectHandlers,init_initHandler(this));
        
        parseEffectWiring();
        storeParameterManagers();
        setEnabledEffects(aEnabledEffects);
    }

    const std::vector<bool>& getEnabledEffects() const
    {
        return mActiveWiringConfig.mEnabledEffects;
    }
    
    void setEnabledEffects(
        const std::vector<bool> &aEnabledEffects)
    {
        RTFACT_ASSERT(aEnabledEffects.size()==EFFECT_COUNT);
        mActiveWiringConfig.mEnabledEffects=aEnabledEffects;
        mMasterWiring=NULL;
        boost::fusion::fold(zipPair(mEnabledHandlers,mEffectHandlers),0,setEnabledEffects_enableHandler<Type>(this));
        recomputeParameterManagers();
    }
    
    const bool isEffectEnabled(
        const size_t aIdxEffect) const
    {
        return mActiveWiringConfig.mEnabledEffects[aIdxEffect];
    }
    
    void setEffectEnabled(
        const size_t aIdxEffect,
        const bool aEnable=true)
    {
        mActiveWiringConfig.mEnabledEffects[aIdxEffect]=aEnable;
        setEnabledEffects(mActiveWiringConfig.mEnabledEffects);
    }

    template <
        typename tColorImage2D,
        typename tDistanceImage2D,
        typename tNormalImage2D,
		typename tAmbientOcclusionImage2D,
        typename tColorBleedingImage2D>
    void fillFBTextures(
        const tColorImage2D &aColorImage,
        const tDistanceImage2D &aDistanceImage,
        const tNormalImage2D &aNormalImage,
		const tAmbientOcclusionImage2D &aAmbientOcclusionImage,
        const tColorBleedingImage2D &aColorBleedingImage,
        const uint aTexX,
        const uint aTexY)
    {
        RTFACT_STATIC_ASSERT((
            boost::is_same<typename tColorImage2D::Component, uint8>::value  ||
            boost::is_same<typename tColorImage2D::Component, float>::value));

        RTFACT_STATIC_ASSERT((            
            boost::is_same<typename tDistanceImage2D::Component, float>::value));

        RTFACT_STATIC_ASSERT((
            boost::is_same<typename tNormalImage2D::Component, float>::value));

		RTFACT_STATIC_ASSERT((            
            boost::is_same<typename tAmbientOcclusionImage2D::Component, float>::value));

        RTFACT_STATIC_ASSERT((
            boost::is_same<typename tColorBleedingImage2D::Component, uint8>::value  ||
            boost::is_same<typename tColorBleedingImage2D::Component, float>::value));

        RTFACT_ASSERT((
            aColorImage.getComponentCount() == 3 || aColorImage.getComponentCount() == 4));

        RTFACT_ASSERT((
            aDistanceImage.getComponentCount() == 1));

        RTFACT_ASSERT((
            aNormalImage.getComponentCount() == 3));

		RTFACT_ASSERT((
            aAmbientOcclusionImage.getComponentCount() == 1));

        RTFACT_ASSERT((
            aColorBleedingImage.getComponentCount() == 3 || aColorBleedingImage.getComponentCount() == 4));
        
        //////////////////////////////////
        // Color Buffer

        const GLenum colorPixelFormat =
            aColorImage.getComponentCount() == 4 ?
                GL_RGBA :
                GL_RGB;

        const_cast<Type*>(this)->validateRenderingSupport();

        GLenum colorChannelType;
        GLenum colorPixelInternalFormat;
        if (boost::is_same<typename tColorImage2D::Component, float>::value)
        {
            colorChannelType=GL_FLOAT;
            colorPixelInternalFormat=(colorPixelFormat==GL_RGBA ? GL_RGBA32F_ARB : GL_RGB32F_ARB);
        }
        else if (boost::is_same<typename tColorImage2D::Component, uint8>::value)
        {
            colorChannelType=GL_UNSIGNED_BYTE;
            colorPixelInternalFormat=colorPixelFormat;
        }
         else
        {
            RTFACT_PROGRAM_ERROR("Invalid format for Color buffer specified. Only float and uint8 are allowed");
        }
        
        if (
            mParameters.mParamTextures[Shader::PARAM_FBCOLOR]!=0)
        {
            glBindTexture(GL_TEXTURE_RECTANGLE_ARB,mParameters.mParamTextures[Shader::PARAM_FBCOLOR]);
            glTexImage2D(
                GL_TEXTURE_RECTANGLE_ARB,
                0,  //no mipmap
                colorPixelInternalFormat,
                aTexX, aTexY,
                0,  //no border
                colorPixelFormat,
                colorChannelType,
                aColorImage.getFirstComponent(0,0));
        }

        //////////////////////////////////
        // Distance Buffer

        if (mParameters.mParamTextures[Shader::PARAM_FBDISTANCE]!=0)
        {
            glBindTexture(GL_TEXTURE_RECTANGLE_ARB,mParameters.mParamTextures[Shader::PARAM_FBDISTANCE]);
            glTexImage2D(
                GL_TEXTURE_RECTANGLE_ARB,
                0,  //no mipmap
				GL_LUMINANCE32F_ARB,// GL_RGB32F_ARB,
                aTexX, aTexY,
                0,  //no border
                GL_LUMINANCE,//GL_RED,
                GL_FLOAT,
                aDistanceImage.getFirstComponent(0,0));
        }
        
        //////////////////////////////////
        // Normal Buffer

        if (mParameters.mParamTextures[Shader::PARAM_FBNORMAL]!=0)
        {
            glBindTexture(GL_TEXTURE_RECTANGLE_ARB,mParameters.mParamTextures[Shader::PARAM_FBNORMAL]);
            glTexImage2D(
                GL_TEXTURE_RECTANGLE_ARB,
                0,  //no mipmap
                GL_RGB32F_ARB,
                aTexX, aTexY,
                0,  //no border
                GL_RGB,
                GL_FLOAT,
                aNormalImage.getFirstComponent(0,0));
        }
        
        //////////////////////////////////
        // Ambient Occlusion Buffer

		if (mParameters.mParamTextures[Shader::PARAM_FBAMBIENTOCCLUSION]!=0)
        {
            glBindTexture(GL_TEXTURE_RECTANGLE_ARB,mParameters.mParamTextures[Shader::PARAM_FBAMBIENTOCCLUSION]);
            glTexImage2D(
                GL_TEXTURE_RECTANGLE_ARB,
                0,  //no mipmap
                GL_LUMINANCE32F_ARB,
                aTexX, aTexY,
                0,  //no border
                GL_LUMINANCE,
                GL_FLOAT,
                aAmbientOcclusionImage.getFirstComponent(0,0));
        }
        
        //////////////////////////////////
        // Color Bleeding Buffer

        const GLenum colorBleedingPixelFormat =
            aColorBleedingImage.getComponentCount() == 4 ?
                GL_RGBA :
                GL_RGB;        

        GLenum colorBleedingChannelType;
        GLenum colorBleedingPixelInternalFormat;
        if (boost::is_same<typename tColorBleedingImage2D::Component, float>::value)
        {
            colorBleedingChannelType=GL_FLOAT;
            colorBleedingPixelInternalFormat = 
                colorBleedingPixelFormat == GL_RGBA ? 
                    GL_RGBA32F_ARB : 
                    GL_RGB32F_ARB;
        }
        else if (boost::is_same<typename tColorBleedingImage2D::Component, uint8>::value)
        {
            colorBleedingChannelType = GL_UNSIGNED_BYTE;
            colorBleedingPixelInternalFormat = colorBleedingPixelFormat;
        }
        else
        {
            RTFACT_PROGRAM_ERROR("Invalid format for ColorBleeding buffer specified. Only float and uint8 are allowed");
        }

		if (mParameters.mParamTextures[Shader::PARAM_FBCOLORBLEEDING] != 0)
        {
            glBindTexture(GL_TEXTURE_RECTANGLE_ARB,mParameters.mParamTextures[Shader::PARAM_FBCOLORBLEEDING]);
            glTexImage2D(
                GL_TEXTURE_RECTANGLE_ARB,
                0,  //no mipmap
                colorBleedingPixelInternalFormat,
                aTexX, aTexY,
                0,  //no border
                colorBleedingPixelFormat,
                colorBleedingChannelType,
                aColorBleedingImage.getFirstComponent(0,0));
        }
    }

    void render(
        const uint aResX,
        const uint aResY,
        const uint aTexX,
        const uint aTexY) const
    {
        const_cast<Type*>(this)->validateRenderingSupport();
        if (
            taPreRender && mHasPreRender)
        {
            boost::fusion::for_each(mPreRenderEffects,render_preRender(this));
        }
        for (
            size_t idxPass=0;idxPass<mMasterWiring->mPasses.size();++idxPass)
        {
            CGpass pass=mMasterWiring->mPasses[idxPass]->mCgPass;
            setAutomaticParameters(idxPass,aTexX,aTexY);
            cgSetPassState(pass);
            if (
                taPrePass && mHasPrePass[idxPass])
            {
                boost::fusion::for_each(mPrePassEffects[idxPass],render_prePass(this,idxPass));
            }
            glClear(mPrePassClear[idxPass]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f,0.0f);
            glVertex2f(0.0f,0.0f);
            glTexCoord2f(float(aTexX),0.0f);
            glVertex2f(float(aResX),0.0f);
            glTexCoord2f(float(aTexX),float(aTexY));
            glVertex2f(float(aResX),float(aResY));
            glTexCoord2f(0.0f,float(aTexY));
            glVertex2f(0.0f,float(aResY));
            glEnd();
            if (
                taPostPass && mHasPostPass[idxPass])
            {
                boost::fusion::for_each(mPostPassEffects[idxPass],render_postPass(this,idxPass));
            }
            swapReadWriteTextures(idxPass);
            cgResetPassState(pass);
            glBindFramebuffer/*ARB*/(GL_DRAW_FRAMEBUFFER/*_ARB*/,0);
        }
        if (
            taPostRender && mHasPostRender)
        {
            boost::fusion::for_each(mPostRenderEffects,render_postRender(this));
        }
    }

    void setCameraPosition(
        const Vec3f1 &aPosition,
        const Vec3f1 &aViewDirection,
        const Vec3f1 &aUp)
    {
        Vec3f1 axisX = Math::cross(aViewDirection,aUp);
        const Vec3f1 &axisZ = aViewDirection;
        Vec3f1 axisY = Math::cross(axisX,axisZ);
        
        mMatrixWorldToEye[0] = axisX.getXf();
        mMatrixWorldToEye[1] = axisY.getXf();
        mMatrixWorldToEye[2] = axisZ.getXf();
        
        mMatrixWorldToEye[3] = axisX.getYf();
        mMatrixWorldToEye[4] = axisY.getYf();
        mMatrixWorldToEye[5] = axisZ.getYf();
        
        mMatrixWorldToEye[6] = axisX.getZf();
        mMatrixWorldToEye[7] = axisY.getZf();
        mMatrixWorldToEye[8] = axisZ.getZf();
        
        updateCameraMatrix();
    }
    
    void setCameraOpeningAngle(
        const float aOpeningAngleDeg)
    {
        mParameters.mFocusDistance = 1.f / Math::tan(Math::radians(aOpeningAngleDeg) * 0.5f);
        
        updateFocusDistance();
    }
    
    const bool isOutputRequested(
        const Shader::Semantic aSemantic) const
    {
        if (
            aSemantic==Shader::PARAM_FBCOLOR)
        {
            return mActiveWiringConfig.mColorOutputRequested;
        }
        else
        {
            return mActiveWiringConfig.mOutputRequested[aSemantic];
        }
    }
    
    void requestOutput(
        const Shader::Semantic aSemantic,
        const bool aRequest=true)
    {
        if (
            aSemantic==Shader::PARAM_FBCOLOR)
        {
            mActiveWiringConfig.mColorOutputRequested=aRequest;
        }
        else
        {
            mActiveWiringConfig.mOutputRequested[aSemantic]=aRequest;
        }
        mMasterWiring=NULL;
    }
    
    void validateRenderingSupport()
    {
        if (
            mMasterWiring==NULL)
        {
            createRenderingSupport();
        }
    }

    #define NUMBER_CASE(maZ,maNumber,maData) \
        case maNumber: BOOST_PP_ARRAY_ELEM(2,maData) BOOST_PP_ARRAY_ELEM(0,maData)<SupportedEffectHandlers, maNumber,boost::mpl::size<tSupportedEffects>::value>::BOOST_PP_ARRAY_ELEM(1,maData)(mEnabledHandlers,mParameters,aParameter,aValue); break;
    
    
    template <
        typename tParameterValue>
    const bool getParameter(
        const Shader::Semantic aParameter,
        tParameterValue &aValue) const
    {
        if (
            !mParameterManagers[aParameter].empty())
        {
            switch (
                mActiveParameterManager[aParameter])
            {
                BOOST_PP_REPEAT(FUSION_MAX_VECTOR_SIZE,NUMBER_CASE,(3,(getParameter_viaHandler,call,return)))
                default:
                    RTFACT_ASSERT(false);
                    return false;
            }
        }
        else
        {
            return false;
        }
    }
    
    template <
        typename tParameterValue>
    const bool getMultiParameter(
        const Shader::Semantic aParameter,
        tParameterValue *aValue) const
    {
        if (
            !mParameterManagers[aParameter].empty())
        {
            switch (
                mActiveParameterManager[aParameter])
            {
                BOOST_PP_REPEAT(FUSION_MAX_VECTOR_SIZE,NUMBER_CASE,(3,(getParameter_viaHandler,callMulti,return)))
                default:
                    RTFACT_ASSERT(false);
                    return false;
            }
        }
        else
        {
            return false;
        }
    }
    
    template <
        typename tParameterValue>
    const bool setParameter(
        const Shader::Semantic aParameter,
        const tParameterValue &aValue) const
    {
        bool ok=true;
        BOOST_FOREACH(
            size_t idx,
            mParameterManagers[aParameter])
        {
            switch (
                idx)
            {
                BOOST_PP_REPEAT(FUSION_MAX_VECTOR_SIZE,NUMBER_CASE,(3,(setParameter_viaHandler,set,ok&=)))
            }
        }
        if (
            !ok)
        {
            return false;
        }
        switch (
            mActiveParameterManager[aParameter])
        {
            BOOST_PP_REPEAT(FUSION_MAX_VECTOR_SIZE,NUMBER_CASE,(3,(setParameter_viaHandler,update,return)))
            default:
                return false;
        }
    }
    
    template <
        typename tParameterValue>
    const bool setMultiParameter(
        const Shader::Semantic aParameter,
        const tParameterValue *aValue) const
    {
        bool ok=true;
        BOOST_FOREACH(
            size_t idx,
            mParameterManagers[aParameter])
        {
            switch (
                idx)
            {
                BOOST_PP_REPEAT(FUSION_MAX_VECTOR_SIZE,NUMBER_CASE,(3,(setParameter_viaHandler,setMulti,ok&=)))
            }
        }
        if (
            !ok)
        {
            return false;
        }
        switch (
            mActiveParameterManager[aParameter])
        {
            BOOST_PP_REPEAT(FUSION_MAX_VECTOR_SIZE,NUMBER_CASE,(3,(setParameter_viaHandler,update,return)))
            default:
                return false;
        }
    }
    
    #undef NUMBER_CASE
    
    const GLuint getOutputGLFramebuffer() const
    {
        return mOutputGLFramebuffer;
    }
    
    void setOutputGLFramebuffer(
        const GLuint aGLFramebuffer)
    {
        if (
            aGLFramebuffer==0 ||
            glIsFramebuffer/*ARB*/(aGLFramebuffer))
        {
            mOutputGLFramebuffer=aGLFramebuffer;
        }
    }
    
    const SupportedParameters& getSupportedParameters() const
    {
        return mSupportedParameters;
    }

    void appendShaderPath(const std::string& aPath)
    {
	    mShaderPaths.push_back(aPath);
    }

    void prependShaderPath(const std::string& aPath)
    {
	    mShaderPaths.push_front(aPath);
    }

    void clearShaderPaths()
    {
	    mShaderPaths.clear();
    }

    IO::PathList getShaderPathList()
    {
	    return mShaderPaths;
    }
};

RTFACT_NAMESPACE_END

#endif // RTFACT__MULTICGFXPOSTPROCESSOR_HPP
