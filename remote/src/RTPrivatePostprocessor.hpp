#ifndef RT_PRIVATE_POSTPROCESSOR_HPP
#define RT_PRIVATE_POSTPROCESSOR_HPP

#include <RTfact/Config/Common.hpp>
#include <RTfact/Model/Image/BasicImage2D.hpp>

#include <RTfact/Model/ShaderEffect/EffectDisplay.hpp>
#include <RTfact/Model/ShaderEffect/EffectGamma.hpp>
#include <RTfact/Model/ShaderEffect/EffectSSAOApply.hpp>
#include <RTfact/Model/ShaderEffect/EffectSSAOBlur.hpp>
#include <RTfact/Model/ShaderEffect/EffectSSAOCompute.hpp>
#include <RTfact/Model/ShaderEffect/EffectRTAOBlur.hpp>
#include <RTfact/Model/ShaderEffect/EffectRTAOApply.hpp>
#include <RTfact/Model/ShaderEffect/EffectRTCBBlur.hpp>
#include <RTfact/Model/ShaderEffect/EffectRTCBApply.hpp>

#include <RTfact/Model/Postprocessor/MultiCgFXPostprocessor.hpp>

#include <RTremote/Config/Common.hpp>

RTFACT_POSTPROCESSING_NAMESPACE_BEGIN

//---------------------------------------------------------------------
// Declaration of supported buffer types
//---------------------------------------------------------------------

//typedef BasicImage2D<uint8>                                 t_ColorImage;
typedef BasicImage2D<float>                                 t_ColorImage;
typedef Traits<t_ColorImage>::Ptr                           t_ColorImagePtr;
typedef ImageTraits<t_ColorImage>::Component                t_ColorImageComponent;

typedef BasicImage2D<float>                                 t_DistanceImage;
typedef Traits<t_DistanceImage>::Ptr                        t_DistanceImagePtr;
typedef ImageTraits<t_DistanceImage>::Component             t_DistanceImageComponent;

typedef BasicImage2D<float>                                 t_NormalImage;
typedef Traits<t_NormalImage>::Ptr                          t_NormalImagePtr;
typedef ImageTraits<t_NormalImage>::Component               t_NormalImageComponent;

typedef BasicImage2D<float>                                 t_AmbientOcclusionImage;
typedef Traits<t_AmbientOcclusionImage>::Ptr                t_AmbientOcclusionImagePtr;
typedef ImageTraits<t_AmbientOcclusionImage>::Component     t_AmbientOcclusionImageComponent;

//typedef BasicImage2D<uint8>                                 t_ColorBleedingImage;
typedef BasicImage2D<float>                                 t_ColorBleedingImage;
typedef Traits<t_ColorBleedingImage>::Ptr                   t_ColorBleedingImagePtr;
typedef ImageTraits<t_ColorBleedingImage>::Component        t_ColorBleedingImageComponent;


//---------------------------------------------------------------------
// Postprocessor Effects Declaration
//---------------------------------------------------------------------

typedef boost::fusion::vector<
    //RTfact::Shader::EffectSSAOCompute,
    //RTfact::Shader::EffectSSAOBlur,
    //RTfact::Shader::EffectSSAOApply,
    RTfact::Shader::EffectRTAOBlur,
    RTfact::Shader::EffectRTAOApply,
    //RTfact::Shader::EffectRTCBBlur,
    //RTfact::Shader::EffectRTCBApply,
    RTfact::Shader::EffectDisplay>                                  t_PostprocessingEffects;

typedef RTfact::MultiCgFXPostprocessor<t_PostprocessingEffects>     t_Postprocessor;

//---------------------------------------------------------------------
// PostprocessorPimpl Declaration
//---------------------------------------------------------------------

class PostprocessorPimpl
{
public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    //---------------------------------------------------------------------
    // Class Fields
    //---------------------------------------------------------------------
    t_Postprocessor mPostprocessor;

    std::vector<EffectHandle> mEffects;
    std::vector<PostprocessorParameterHandle> mParameters;

    t_ColorImagePtr mColorImagePtr;
    t_DistanceImagePtr mDistanceImagePtr;
    t_NormalImagePtr mNormalImagePtr;
    t_AmbientOcclusionImagePtr mAmbientOcclusionImagePtr;
    t_ColorBleedingImagePtr mColorBleedingImagePtr;

    Vec3f1 mCameraPosition;
    Vec3f1 mCameraViewDirection;
    Vec3f1 mCameraUpVector;
    float mCameraOpeningAngle;

    //---------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------
    PostprocessorPimpl();
    void initEffects();    
};

//---------------------------------------------------------------------
// EffectPimpl Declaration
//---------------------------------------------------------------------

class EffectPimpl
{
public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    //---------------------------------------------------------------------
    // Class Fields
    //---------------------------------------------------------------------
    PostprocessorPimpl &mPostprocessor;

    std::string mName;

    //---------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------
    EffectPimpl(
        PostprocessorPimpl &aPostprocessor,
        const std::string &aName);
};

//---------------------------------------------------------------------
// PostprocessorParameterPimpl Declaration
//---------------------------------------------------------------------

class PostprocessorParameterPimpl
{
public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    //---------------------------------------------------------------------
    // Class Fields
    //---------------------------------------------------------------------
    PostprocessorPimpl &mPostprocessor;

    RTfact::Shader::Semantic mSemantic;
    size_t mComponentCount;

    //---------------------------------------------------------------------
    // Functions
    //---------------------------------------------------------------------
    PostprocessorParameterPimpl(
        PostprocessorPimpl &aPostprocessor);	
};

RTFACT_POSTPROCESSING_NAMESPACE_END

#endif // RT_PRIVATE_POSTPROCESSOR_HPP
