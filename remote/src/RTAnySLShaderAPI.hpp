#ifndef _SHADER_API
#define _SHADER_API

#include <RTfact/Config/Common.hpp>


#if defined(RTFACT_COMPILER_MSVC) || defined(RTFACT_COMPILER_INTEL)
#  pragma message ("GCC attribute for realigning: wont work with MSVC and INTEL compilers !!")
#  define _REALIGN
#else
#  define _REALIGN __attribute__((force_align_arg_pointer))
#endif

#include <RTfact/Concept/Light.hpp>
#include <RTfact/Utils/Math/SamplingUtils.hpp>
#include <RTfact/Utils/ShadingUtils.hpp>
#include <RTfact/Utils/SurfaceShaderContext.hpp>
#include <RTfact/Model/Image/BasicImage2D.hpp>
#include <RTfact/Model/ImageSampler/BilinearFilterImageSampler.hpp>
#include <RTfact/Model/LightAttenuation/QuadraticLightAttenuation.hpp>
#include "RTImageImpl.hpp"
#include <RTAnySL/Utils.hpp>
#include <boost/pointer_cast.hpp>

// When ANYSL_RTFACT_BYTE_AND_FLOAT_TEXTURE_SUPPORT is defined type of
// the texture representation is const RTImageImpl*, otherwise only
// float textures are supported and type is const BasicImage2D<float>*.
#define ANYSL_RTFACT_BYTE_AND_FLOAT_TEXTURE_SUPPORT 1

RTFACT_REMOTE_NAMESPACE_BEGIN

struct t_Vec3f1 { float x, y, z; } RTFACT_ALIGN_SIMD; //alignment required for casting to __m128

template<class tContext>
struct ShaderAPI
{
	static void printv(const RTfact::Vec3f<1> & v)
	{
		printf("(%f, %f, %f)", v.xAt(0), v.yAt(0), v.zAt(0));
	}

	/* Vec3f used in scalar shaders */
	struct ShaderVec3f
	{
		float x,y,z;

		void set(float a, float b, float c)
		{
			x=a;y=b;z=c;
		}

		void operator=(const Vec3f<1> & src)
		{
			x = src.xAt(0);
			y = src.yAt(0);
			z = src.zAt(0);
		}

		operator RTfact::Vec3f<1>() const
		{
			return RTfact::Vec3f<1>(x, y, z);
		}

		void print() const
		{
			printf("(%f, %f, %f)", x, y, z);
		}
	};

	typedef Packet<1, float>         Float1;

	typedef Vec3f<4> Vec3f4;
	typedef Packet<4, int> Int4;
	typedef Packet<4, float> Float4;
	typedef ActiveMask<4> ShaderMask;

	/* API Calls */
	//############################################# Scalar calls ##########################################
	static void trace_1
	(
		tContext       * __restrict aContext,
		const float                 aPositionX,
		const float                 aPositionY,
		const float                 aPositionZ,
		const float                 aDirectionX,
		const float                 aDirectionY,
		const float                 aDirectionZ,
		const unsigned              aDepth,
		float          * __restrict oResultX,
		float          * __restrict oResultY,
		float          * __restrict oResultZ
	) _REALIGN;

  static void traceParticle_1
  (
	  tContext       * __restrict aContext,
	  const float                 aPositionX, //no eps added yet!
	  const float                 aPositionY, //no eps added yet!
	  const float                 aPositionZ, //no eps added yet!
	  const float                 aDirectionX,
	  const float                 aDirectionY,
	  const float                 aDirectionZ,
	  const float                 aCumulativeFactorX,
	  const float                 aCumulativeFactorY,
	  const float                 aCumulativeFactorZ,
    const int                   aPrimeBase,
    const int                   aRandomSeed,
    const bool                  aInside,
	  const unsigned              aDepth, //not incremented yet!
	  float          * __restrict oResultX,
	  float          * __restrict oResultY,
	  float          * __restrict oResultZ
  ) _REALIGN;

	static void getLC_1
	(
		tContext    * __restrict aContext,
		const float              aPositionX,
		const float              aPositionY,
		const float              aPositionZ,
		const float              aDirectionX, //not negated yet!
		const float              aDirectionY, //not negated yet!
		const float              aDirectionZ, //not negated yet!
		const float              aDistance,
		unsigned    * __restrict oContributes, //not yet set to 0!
		float       * __restrict oIntensityX,
		float       * __restrict oIntensityY,
		float       * __restrict oIntensityZ
	) _REALIGN;

	static void sampleLight_1
	(
		tContext       * __restrict aContext,
		const unsigned              aLightIndex,
		const float                 aPointX,
		const float                 aPointY,
		const float                 aPointZ,
		float          * __restrict oDirectionX,
		float          * __restrict oDirectionY,
		float          * __restrict oDirectionZ,
		float          * __restrict oDistance,
		float          * __restrict oRadianceX,
		float          * __restrict oRadianceY,
		float          * __restrict oRadianceZ,
		float          * __restrict oPositionX,
		float          * __restrict oPositionY,
		float          * __restrict oPositionZ
	) _REALIGN;

  static bool getLightIntensity_1
  (
      const float aPointX,
      const float aPointY,
      const float aPointZ,
      const float aNormalX,
      const float aNormalY,
      const float aNormalZ,
      const RTfact::Light * __restrict aLight,
	    tContext * __restrict aContext,
      float * __restrict oDirToLightX,
      float * __restrict oDirToLightY,
      float * __restrict oDirToLightZ,
	    float * __restrict oIntensityX,
      float * __restrict oIntensityY,
      float * __restrict oIntensityZ,
	    float * __restrict oDotLightNormal,
      bool * __restrict oLightMask
  ) _REALIGN;

  static bool getLightSourceIntensity_1
  (
      tContext * __restrict aContext,
      const int aLightIndex,
      const float aPointX,
      const float aPointY,
      const float aPointZ,
      const float aNormalX,
      const float aNormalY,
      const float aNormalZ,
      float * __restrict oDirToLightX,
      float * __restrict oDirToLightY,
      float * __restrict oDirToLightZ,
      float * __restrict oIntensityX,
      float * __restrict oIntensityY,
      float * __restrict oIntensityZ,
      float * __restrict oDotLightNormal
  ) _REALIGN;

	static void sampleDT_1
	(
		tContext    * __restrict aContext,
		const void  * __restrict aTextureHandle,
		const float              aTexCoordX,
		const float              aTexCoordY,
		const float              aTexCoordOffsetX,
		const float              aTexCoordOffsetY,
		float       * __restrict oResultX,
		float       * __restrict oResultY,
		float       * __restrict oResultZ
	) _REALIGN;

	static void sampleDT_id_1
	(
		tContext       * __restrict aContext,
		void           * __restrict aTextureTable,
		const unsigned              aTextureID,
		const float                 aTexCoordX,
		const float                 aTexCoordY,
		const float                 aTexCoordOffsetX,
		const float                 aTexCoordOffsetY,
		float          * __restrict oResultX,
		float          * __restrict oResultY,
		float          * __restrict oResultZ
	) _REALIGN;

	static unsigned getNumLight
	(
		tContext * __restrict aContext
	);

  static unsigned getLightPathCount
  (
	  tContext * __restrict aContext
  );

    static float getTime
    (
        tContext * __restrict aContext
    );

    static bool getParticlesGenerated
    (
        tContext * __restrict aContext
    );

    static void insertNewImporton
    (
	      tContext * __restrict aContext,
	      const float hitX,
	      const float hitY,
	      const float hitZ,
	      const float intensityX,
	      const float intensityY,
	      const float intensityZ,
	      const float normalX,
	      const float normalY,
	      const float normalZ
    ) _REALIGN;

    static void assessParticle
    (
	      tContext * __restrict aContext,
	      const float hitX,
	      const float hitY,
	      const float hitZ,
	      const float intensityX,
	      const float intensityY,
	      const float intensityZ,
	      const float normalX,
	      const float normalY,
	      const float normalZ
    ) _REALIGN;

    static void initParticleQuery(
	    tContext * __restrict aContext,
        unsigned * pqHandle
    ) _REALIGN;

    static bool getNextParticle(
        tContext * __restrict aContext,
        unsigned * pqHandle,
        void  ** __restrict particle,
	      float * __restrict posX,
	      float * __restrict posY,
	      float * __restrict posZ,
        float * __restrict intensity
    ) _REALIGN;

    static float getParticleGeneratorClampThreshold(
        tContext * __restrict aContext
    ) _REALIGN;

    static void incrementTotalFrameLuminance(
        tContext * __restrict aContext,
        float frameLuminanceContribution
    ) _REALIGN;

	//################################## Vectorized Calls ##########################################
	static void trace_4(
		tContext       * __restrict aContext,
		const __m128i               mask,
		const __m128                aPositionX,
		const __m128                aPositionY,
		const __m128                aPositionZ,
		const __m128                aDirectionX,
		const __m128                aDirectionY,
		const __m128                aDirectionZ,
		const unsigned              aDepth,
		__m128         * __restrict oResultX,
		__m128         * __restrict oResultY,
		__m128         * __restrict oResultZ
	) _REALIGN;

	static void getLC_4(
		tContext      * __restrict aContext,
		const __m128i              mask,
		const __m128               aPositionX,
		const __m128               aPositionY,
		const __m128               aPositionZ,
		const __m128               aDirectionX, //not negated yet!
		const __m128               aDirectionY, //not negated yet!
		const __m128               aDirectionZ, //not negated yet!
		const __m128               aDistance,
		__m128i       * __restrict oContributes, //not yet set to 0!
		__m128        * __restrict oIntensityX,
		__m128        * __restrict oIntensityY,
		__m128        * __restrict oIntensityZ
	) _REALIGN;

	static void sampleLight_4(
		tContext       * __restrict aContext,
		const __m128i               mask,
		const unsigned              aLightIndex,
		const __m128                aPointX,
		const __m128                aPointY,
		const __m128                aPointZ,
		__m128         * __restrict oDirectionX,
		__m128         * __restrict oDirectionY,
		__m128         * __restrict oDirectionZ,
		__m128         * __restrict oDistance,
		__m128         * __restrict oRadianceX,
		__m128         * __restrict oRadianceY,
		__m128         * __restrict oRadianceZ,
		__m128         * __restrict oPositionX,
		__m128         * __restrict oPositionY,
		__m128         * __restrict oPositionZ
	) _REALIGN;

	static void sampleDT_4
	(
		tContext      * __restrict aContext,
		const __m128i              mask,
		const void    * __restrict aTextureHandle,
		const __m128               aTexCoordX,
		const __m128               aTexCoordY,
		const __m128               aTexCoordOffsetX,
		const __m128               aTexCoordOffsetY,
		__m128        * __restrict oResultX,
		__m128        * __restrict oResultY,
		__m128        * __restrict oResultZ
	) _REALIGN;

	static void sampleDT_id_4
	(
		tContext       * __restrict aContext,
		const __m128i               mask,
		void           * __restrict aTextureTable,
		const unsigned              aTextureID,
		const __m128                aTexCoordX,
		const __m128                aTexCoordY,
		const __m128                aTexCoordOffsetX,
		const __m128                aTexCoordOffsetY,
		__m128         * __restrict oResultX,
		__m128         * __restrict oResultY,
		__m128         * __restrict oResultZ
	) _REALIGN;


	//############################### Linking ############################

    static const RTAnySL::APIFunc * getAPIFuncTable()
    {
        RTANYSL_BEGIN_API_TABLE(apiFuncTable)
        // single ray
        RTANYSL_API_FUNC("RTFACT_trace_1", trace_1)
        RTANYSL_API_FUNC("RTFACT_traceParticle_1", traceParticle_1)
        RTANYSL_API_FUNC("RTFACT_getLC_1", getLC_1)
        RTANYSL_API_FUNC("RTFACT_sampleLight_1", sampleLight_1)
        RTANYSL_API_FUNC("RTFACT_getLightIntensity_1", getLightIntensity_1)
        RTANYSL_API_FUNC("RTFACT_getLightSourceIntensity_1", getLightSourceIntensity_1)
        RTANYSL_API_FUNC("RTFACT_sampleDT_1", sampleDT_1)
        RTANYSL_API_FUNC("RTFACT_sampleDT_id_1", sampleDT_id_1)
        RTANYSL_API_FUNC("RTFACT_getNumLight", getNumLight)
        RTANYSL_API_FUNC("RTFACT_getLightPathCount", getLightPathCount)
        RTANYSL_API_FUNC("RTFACT_getTime", getTime)
        RTANYSL_API_FUNC("RTFACT_getParticlesGenerated", getParticlesGenerated)
        RTANYSL_API_FUNC("RTFACT_insertNewImporton", insertNewImporton)
        RTANYSL_API_FUNC("RTFACT_assessParticle", assessParticle)
        RTANYSL_API_FUNC("RTFACT_initParticleQuery", initParticleQuery)
        RTANYSL_API_FUNC("RTFACT_getNextParticle", getNextParticle)
        RTANYSL_API_FUNC("RTFACT_getParticleGeneratorClampThreshold", getParticleGeneratorClampThreshold)
        RTANYSL_API_FUNC("RTFACT_incrementTotalFrameLuminance", incrementTotalFrameLuminance)

        // packetized
        RTANYSL_API_FUNC("RTFACT_trace_4", trace_4)
        RTANYSL_API_FUNC("RTFACT_getLC_4", getLC_4)
        RTANYSL_API_FUNC("RTFACT_sampleLight_4", sampleLight_4)
        RTANYSL_API_FUNC("RTFACT_sampleDT_4", sampleDT_4)
        RTANYSL_API_FUNC("RTFACT_sampleDT_id_4", sampleDT_id_4)
        RTANYSL_END_API_TABLE()

        return apiFuncTable;
    }

	/*
	 * links the api calls to RTfact
	 */
	static void linkAPI(llvm::Module * shaderModule)
	{
        RTAnySL::Utils::linkAPIFuncTable(shaderModule, getAPIFuncTable());
	}
};


template<class tContext>
void ShaderAPI<tContext>::trace_1
(
	tContext       * __restrict aContext,
	const float                 aPositionX, //no eps added yet!
	const float                 aPositionY, //no eps added yet!
	const float                 aPositionZ, //no eps added yet!
	const float                 aDirectionX,
	const float                 aDirectionY,
	const float                 aDirectionZ,
	const unsigned              aDepth, //not incremented yet!
	float          * __restrict oResultX,
	float          * __restrict oResultY,
	float          * __restrict oResultZ
)
{
	typedef RTfact::Packet<1, float>::Container t_FPacketC1;

	//prepare
	RTfact::RayPacket<1> ray;
	ray.org = RTfact::Vec3f<1>(aPositionX, aPositionY, aPositionZ);
	ray.dir = RTfact::Vec3f<1>(aDirectionX, aDirectionY, aDirectionZ);
	ray.org += ray.dir * t_FPacketC1::C_RAY_EPS; //add epsilon to prevent precision-issues

	ray.invDir = ray.dir.reciprocal();
	ray.tMin = t_FPacketC1::C_RAY_EPS;
	ray.tMax = t_FPacketC1::C_INFINITY;

	ray.depth = aDepth+1;

	ActiveMask<1> aRayMask = ActiveMask<1>::C_TRUE;
	aRayMask.setTrueAndUpdateActiveContainers();

	RTfact::Vec3f<1> tmpResult;
	aContext->template trace<false, false, 1>(ray, aRayMask, tmpResult);
	*oResultX = tmpResult.x;
	*oResultY = tmpResult.y;
	*oResultZ = tmpResult.z;
}

template<class tContext>
void ShaderAPI<tContext>::traceParticle_1
(
	tContext       * __restrict aContext,
	const float                 aPositionX, //no eps added yet!
	const float                 aPositionY, //no eps added yet!
	const float                 aPositionZ, //no eps added yet!
	const float                 aDirectionX,
	const float                 aDirectionY,
	const float                 aDirectionZ,
	const float                 aCumulativeFactorX,
	const float                 aCumulativeFactorY,
	const float                 aCumulativeFactorZ,
  const int                   aPrimeBase,
  const int                   aRandomSeed,
  const bool                  aInside,
	const unsigned              aDepth, //not incremented yet!
	float          * __restrict oResultX,
	float          * __restrict oResultY,
	float          * __restrict oResultZ
)
{
	typedef RTfact::Packet<1, float>::Container t_FPacketC1;

	//prepare
	ActiveMask<1> aRayMask = ActiveMask<1>::C_TRUE;
	aRayMask.setTrueAndUpdateActiveContainers();

  RTfact::RayPacket<1> bouncedParticle;
  bouncedParticle.dir(0) = RTfact::Vec3f<1>(aDirectionX, aDirectionY, aDirectionZ);
  bouncedParticle.dir(0).setNormalized();
  bouncedParticle.invDir(0) = bouncedParticle.dir(0).reciprocal();
  bouncedParticle.org(0) = RTfact::Vec3f<1>(aPositionX, aPositionY, aPositionZ);
  bouncedParticle.tMin(0) = t_FPacketC1::C_RAY_EPS;
  bouncedParticle.tMax(0) = t_FPacketC1::C_INFINITY;
  bouncedParticle.cumulativeFactor(0) = RTfact::Vec3f<1>(aCumulativeFactorX, aCumulativeFactorY, aCumulativeFactorZ);
  bouncedParticle.primeBase(0) = aPrimeBase;
  bouncedParticle.depth = aDepth + 1;
  bouncedParticle.randomSeed = aRandomSeed;
  bouncedParticle.rayType = RayType::ParticleRay;
  bouncedParticle.inside = aInside;

	RTfact::Vec3f<1> tmpResult;
  aContext->template trace<false, false>(bouncedParticle, aRayMask, tmpResult);

	*oResultX = tmpResult.x;
	*oResultY = tmpResult.y;
	*oResultZ = tmpResult.z;
}

template<class tContext>
void ShaderAPI<tContext>::getLC_1
(
	tContext    * __restrict aContext,
	const float              aPositionX,
	const float              aPositionY,
	const float              aPositionZ,
	const float              aDirectionX, //not negated yet!
	const float              aDirectionY, //not negated yet!
	const float              aDirectionZ, //not negated yet!
	const float              aDistance,
	unsigned    * __restrict oContributes, //not yet set to 0!
	float       * __restrict oIntensityX,
	float       * __restrict oIntensityY,
	float       * __restrict oIntensityZ
)
{
	typedef RTfact::Packet<1, float>::Container t_FPacketC1;

	RTfact::RayPacket<1> ray;
	ray.dir = RTfact::Vec3f<1>(-aDirectionX, -aDirectionY, -aDirectionZ);
	ray.org = RTfact::Vec3f<1>(aPositionX, aPositionY, aPositionZ);

	ray.invDir = ray.dir.reciprocal();
	ray.tMax = aDistance - t_FPacketC1::C_RAY_EPS;
	ray.tMin = t_FPacketC1::C_RAY_EPS;

	//set default 'contributes'-mask
	ActiveMask<1> aRayMask = ActiveMask<1>::C_TRUE;
	aRayMask.setTrueAndUpdateActiveContainers(); //required?

	RTfact::Vec3f<1> tmpIntensity(*oIntensityX, *oIntensityY, *oIntensityZ);
	RTfact::ActiveMask<1> ctr;
	ctr.setFalseAndUpdateActiveContainers();
	aContext->template getLightContribution<false, 1>(ray, aRayMask, ctr, tmpIntensity);
	*oContributes = ctr.isTrue();
	*oIntensityX = tmpIntensity.x;
	*oIntensityY = tmpIntensity.y;
	*oIntensityZ = tmpIntensity.z;
}

template<class tContext>
void ShaderAPI<tContext>::sampleLight_1
(
	tContext       * __restrict aContext,
	const unsigned              aLightIndex,
	const float                 aPointX,
	const float                 aPointY,
	const float                 aPointZ,
	float          * __restrict oDirectionX,
	float          * __restrict oDirectionY,
	float          * __restrict oDirectionZ,
	float          * __restrict oDistance,
	float          * __restrict oRadianceX,
	float          * __restrict oRadianceY,
	float          * __restrict oRadianceZ,
	float          * __restrict oPositionX,
	float          * __restrict oPositionY,
	float          * __restrict oPositionZ
)
{
	RTfact::Vec3f<1> p(aPointX, aPointY, aPointZ);
	RTfact::Vec3f<1> dir(*oDirectionX, *oDirectionY, *oDirectionZ);
	RTfact::Vec3f<1> rad(*oRadianceX, *oRadianceY, *oRadianceZ);
	RTfact::Vec3f<1> pos(*oPositionX, *oPositionY, *oPositionZ);

	ActiveMask<1> aRayMask = ActiveMask<1>::C_TRUE;
	aRayMask.setTrueAndUpdateActiveContainers();

//	aContext->scene.lights[aLightIndex]->template sample<1>(
//		p,
//		*(RTfact::Vec3f<1>*)oDirection,
//		*(Packet<1, float>*)oDistance,
//		*(RTfact::Vec3f<1>*)oRadiance,
//		aRayMask,
//		(RTfact::Vec3f<1>*)oPosition
//	);
	/*aContext->scene.lights[aLightIndex]->template sample<1>(
		p,
		dir,
		*(Packet<1, float>*)oDistance,
		rad,
		aRayMask,
		&pos
	);*/
	aContext->scene.lights[aLightIndex]->template illuminatePoint<1>(
        Math::uniformFloat2D<1>(),
	    p,
	    aRayMask,
	    dir,
	    *(Packet<1, float>*)oDistance,
	    rad,
	    &pos
	);
	*oDirectionX = dir.x;
	*oDirectionY = dir.y;
	*oDirectionZ = dir.z;
	*oRadianceX = rad.x;
	*oRadianceY = rad.y;
	*oRadianceZ = rad.z;
	*oPositionX = pos.x;
	*oPositionY = pos.y;
	*oPositionZ = pos.z;
}


template<
    bool taGetColor,
    bool taGetTransparency,
    class tImage,
    uint taSize>
RTFACT_TEXTURE_INLINE void sampleTexture(
    const tImage* aImage,
    const Packet<taSize, float>& aX,
    const Packet<taSize, float>& aY,
    const ActiveMask<taSize>& aMask,
    Vec3f<taSize>* oColor,
    Packet<taSize, float>* oAlpha = RTFACT_NULL)
{
    typedef typename Packet<taSize, float>::Container t_FPacketC;
    typedef typename Packet<taSize, int>::Container   t_IPacketC;
    typedef typename Vec3f<taSize>::Container         t_Vec3fC;
    typedef typename Mask<taSize>::Container          t_MaskC;

    RTFACT_TEXTURE_STATIC_ASSERT(taGetTransparency || taGetColor);

    RTFACT_TEXTURE_ASSERT(!taGetColor        || oColor != RTFACT_NULL);
    RTFACT_TEXTURE_ASSERT(!taGetTransparency || oAlpha != RTFACT_NULL);

    //RTFACT_LOG_LINE_F("Texcoords: %1%, %2%", aX.data % aY.data);

    if(aImage != RTFACT_NULL)
    {
        const t_FPacketC resXC =
            t_FPacketC::replicate(static_cast<float>(aImage->getResX()));

        const t_FPacketC resYC =
            t_FPacketC::replicate(static_cast<float>(aImage->getResY()));

        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            t_FPacketC coordXFraction =
                Math::fraction(aX(i));

            coordXFraction = (coordXFraction < t_FPacketC::C_0()).iif(
                t_FPacketC::C_1 + coordXFraction,
                coordXFraction);

            coordXFraction = Math::min(coordXFraction, t_FPacketC::C_1_MINUS_EPS);

            t_FPacketC coordYFraction =
                Math::fraction(aY(i));

            coordYFraction = (coordYFraction < t_FPacketC::C_0()).iif(
                t_FPacketC::C_1 + coordYFraction,
                coordYFraction);

            coordYFraction = Math::min(coordYFraction, t_FPacketC::C_1_MINUS_EPS);

            if(taGetTransparency)
            {
                if(taGetColor)
                {
                    BilinearFilterImageSampler::template sample<
                        taGetColor, taGetTransparency>(
                            *aImage, coordXFraction, coordYFraction, aMask(i),
                            &(*oColor)(i), &(*oAlpha)(i));
                }
                else
                {
                    BilinearFilterImageSampler::template sample<
                        taGetColor, taGetTransparency>(
                            *aImage, coordXFraction, coordYFraction, aMask(i),
                            (t_Vec3fC*) 0, &(*oAlpha)(i));
                }
            }
            else
            {
                BilinearFilterImageSampler::template sample<
                    taGetColor, taGetTransparency>(
                        *aImage, coordXFraction, coordYFraction, aMask(i),
                        &(*oColor)(i));
            }
        }
    }
    else
    {
        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            if(taGetColor)
            {
                (*oColor)(i) = aMask(i).iif(t_Vec3fC::C_1_0_1, (*oColor)(i));
            }

            if(taGetTransparency)
            {
                (*oAlpha)(i) = aMask(i).iif(t_FPacketC::C_1, (*oAlpha)(i));
            }
        }
    }
}

template<
    uint taSize>
RTFACT_TEXTURE_INLINE void sampleRTImageImpl(
    const RTfact::Remote::RTImageImpl *image,
    const RTfact::Packet<taSize, float>& aX,
    const RTfact::Packet<taSize, float>& aY,
    const RTfact::ActiveMask<taSize>& aMask,
    RTfact::Vec3f<taSize> & oColor)
{
    if (!image)
    {
        typedef typename Vec3f<taSize>::Container t_Vec3fC;
        RTFACT_PACKET_FOR_ALL_ACTIVE_CONTAINERS(i, aMask)
        {
            if (true /*taGetColor*/)
            {
                (oColor)(i) = aMask(i).iif(t_Vec3fC::C_1_0_1, (oColor)(i));
            }
        }
    }
    else
    {
        const RTfact::Traits<Image2D>::Ptr &rtfactImage = image->getRTfactImage();
        if (image->getComponentType() == RTImage::COMPONENT_BYTE)
        {
            typedef RTfact::BasicImage2D<unsigned char> t_ImageB;

            t_ImageB* byteImage = boost::static_pointer_cast<t_ImageB>(rtfactImage.get());
            sampleTexture<true, false>(
                    byteImage,
                    aX,
                    aY,
                    aMask,
                    &oColor
            );
        }
        else if (image->getComponentType() == RTImage::COMPONENT_FLOAT)
        {
            typedef RTfact::BasicImage2D<float> t_ImageF;

            t_ImageF* floatImage = boost::static_pointer_cast<t_ImageF>(rtfactImage.get());
            sampleTexture<true, false>(
                    floatImage,
                    aX,
                    aY,
                    aMask,
                    &oColor);
        }
    }
}

template<class tContext>
bool ShaderAPI<tContext>::getLightIntensity_1
(
    const float aPointX,
    const float aPointY,
    const float aPointZ,
    const float aNormalX,
    const float aNormalY,
    const float aNormalZ,
    const RTfact::Light * __restrict aLight,
	  tContext * __restrict aContext,
    float * __restrict oDirToLightX,
    float * __restrict oDirToLightY,
    float * __restrict oDirToLightZ,
    float * __restrict oIntensityX,
    float * __restrict oIntensityY,
    float * __restrict oIntensityZ,
    float * __restrict oDotLightNormal,
    bool * __restrict oLightMask)
{
    const Vec3f1 point(aPointX, aPointY, aPointZ);
    const Vec3f1 normal(aNormalX, aNormalY, aNormalZ);
    ActiveMask<1> rayMask = ActiveMask<1>::C_TRUE;
	  rayMask.setTrueAndUpdateActiveContainers();
    ActiveMask<1> lightMask;
    Vec3f1 dirToLight;
    Vec3f1 intensity;
    RTfact::Packet<1, float> dotLightNormal;

    bool retval = RTfact::getLightIntensity<1,tContext>(
        point,
        normal,
        rayMask,
        *aLight,
        *aContext,
        dirToLight,
        intensity,
        dotLightNormal,
        lightMask);

    *oDotLightNormal = dotLightNormal(0);
    *oDotLightNormal = RTfact::Math::dot(dirToLight, normal);
    *oLightMask = lightMask(0);
    *oDirToLightX = dirToLight.x;
    *oDirToLightY = dirToLight.y;
    *oDirToLightZ = dirToLight.z;
    *oIntensityX = intensity.x;
    *oIntensityY = intensity.y;
    *oIntensityZ = intensity.z;

    return retval;
}

template<class tContext>
bool ShaderAPI<tContext>::getLightSourceIntensity_1
(
    tContext * __restrict aContext,
    const int aLightIndex,
    const float aPointX,
    const float aPointY,
    const float aPointZ,
    const float aNormalX,
    const float aNormalY,
    const float aNormalZ,
    float * __restrict oDirToLightX,
    float * __restrict oDirToLightY,
    float * __restrict oDirToLightZ,
    float * __restrict oIntensityX,
    float * __restrict oIntensityY,
    float * __restrict oIntensityZ,
    float * __restrict oDotLightNormal)
{
    const Vec3f1 point(aPointX, aPointY, aPointZ);
    const Vec3f1 normal(aNormalX, aNormalY, aNormalZ);
    ActiveMask<1> rayMask = ActiveMask<1>::C_TRUE;
	  rayMask.setTrueAndUpdateActiveContainers();
    ActiveMask<1> lightMask;
    Vec3f1 dirToLight;
    Vec3f1 intensity;
    RTfact::Packet<1, float> dotLightNormal;

    bool retval = RTfact::getLightIntensity<1,tContext>(
        point,
        normal,
        rayMask,
        *(*aContext).scene.lights[aLightIndex],
        *aContext,
        dirToLight,
        intensity,
        dotLightNormal,
        lightMask);

    *oDotLightNormal = dotLightNormal(0);
    *oDotLightNormal = RTfact::Math::dot(dirToLight, normal);
    *oDirToLightX = dirToLight.x;
    *oDirToLightY = dirToLight.y;
    *oDirToLightZ = dirToLight.z;
    *oIntensityX = intensity.x;
    *oIntensityY = intensity.y;
    *oIntensityZ = intensity.z;

    return retval;
}

template<class tContext>
void ShaderAPI<tContext>::sampleDT_1
(
	tContext    * __restrict aContext,
	const void  * __restrict aTextureHandle,
	const float              aTexCoordX,
	const float              aTexCoordY,
	const float              aTexCoordOffsetX,
	const float              aTexCoordOffsetY,
	float       * __restrict oResultX,
	float       * __restrict oResultY,
	float       * __restrict oResultZ
)
{
    ActiveMask<1> aRayMask = ActiveMask<1>::C_TRUE;
    aRayMask.setTrueAndUpdateActiveContainers();

#ifndef ANYSL_RTFACT_BYTE_AND_FLOAT_TEXTURE_SUPPORT
    const BasicImage2D<float>* image = reinterpret_cast<const BasicImage2D<float>*>(aTextureHandle);

	RTfact::Vec3f<1> tmpResult;
    sampleTexture<true, false>(image, *((Packet<1, float>*) &aTexCoordX), *((Packet<1, float>*) &aTexCoordY), aRayMask, &tmpResult);
#else
    const RTImageImpl* image = reinterpret_cast<const RTImageImpl*>(aTextureHandle);

    RTfact::Vec3f<1> tmpResult;

    sampleRTImageImpl(
        image,
        *((RTfact::Packet<1, float>*) &aTexCoordX),
        *((RTfact::Packet<1, float>*) &aTexCoordY),
        aRayMask,
        tmpResult);
#endif

	*oResultX = tmpResult.x;
	*oResultY = tmpResult.y;
	*oResultZ = tmpResult.z;
}
template<class tContext>
void ShaderAPI<tContext>::sampleDT_id_1
(
	tContext       * __restrict aContext,
	void           * __restrict aTextureTable,
	const unsigned              aTextureID,
	const float                 aTexCoordX,
	const float                 aTexCoordY,
	const float                 aTexCoordOffsetX,
	const float                 aTexCoordOffsetY,
	float          * __restrict oResultX,
	float          * __restrict oResultY,
	float          * __restrict oResultZ
)
{
    // In aTextureTable pointer to the texture data 
	// table is stored. Texture table is an array of void *,
    // where each element points to internal texture data.
	if (aTextureTable) {
	    sampleDT_1(aContext,
	               reinterpret_cast<void**>(aTextureTable)[aTextureID],
	               aTexCoordX,
	               aTexCoordY,
	               aTexCoordOffsetX,
	               aTexCoordOffsetY,
	               oResultX, oResultY, oResultZ);
	} else {
		*oResultX = 0.f;
		*oResultY = 1.f;
		*oResultZ = 0.f;
	}
}

template<class tContext>
unsigned ShaderAPI<tContext>::getNumLight
(
	tContext * __restrict aContext
)
{
	return static_cast<unsigned>(aContext->scene.lights.size());
}

template<class tContext>
unsigned ShaderAPI<tContext>::getLightPathCount
(
	tContext * __restrict aContext
)
{
	return static_cast<unsigned>(aContext->particleGenerator.getLightPathCount());
}

template<class tContext>
float ShaderAPI<tContext>::getTime
(
    tContext * __restrict aContext
)
{
    return aContext->time;
};

template<class tContext>
bool ShaderAPI<tContext>::getParticlesGenerated
(
	  tContext * __restrict aContext
)
{
    return aContext->particleGenerator.getParticlesGenerated();
};

template<class tContext>
void ShaderAPI<tContext>::insertNewImporton
(
	  tContext * __restrict aContext,
	  const float hitX,
	  const float hitY,
	  const float hitZ,
	  const float intensityX,
	  const float intensityY,
	  const float intensityZ,
	  const float normalX,
	  const float normalY,
	  const float normalZ
)
{
    RTfact::Vec3f<1> hit(hitX, hitY, hitZ);
    RTfact::Vec3f<1> intensity(intensityX, intensityY, intensityZ);
    RTfact::Vec3f<1> normal(normalX, normalY, normalZ);

    typename tContext::tImporton newImporton(hit, intensity, normal);

    aContext->particleFilter.importonStorage.insertParticle(newImporton);
}

template<class tContext>
void ShaderAPI<tContext>::assessParticle
(
	  tContext * __restrict aContext,
	  const float hitX,
	  const float hitY,
	  const float hitZ,
	  const float intensityX,
	  const float intensityY,
	  const float intensityZ,
	  const float normalX,
	  const float normalY,
	  const float normalZ
)
{
    RTfact::QuadraticLightAttenuation particleAttn;

    RTfact::Vec3f<1> hit(hitX, hitY, hitZ);
    RTfact::Vec3f<1> intensity(intensityX, intensityY, intensityZ);
    RTfact::Vec3f<1> normal(normalX, normalY, normalZ);

    typename tContext::tParticle newParticle(hit, intensity, normal, particleAttn);
    aContext->assessParticle(newParticle);
}

template<class tContext>
void ShaderAPI<tContext>::initParticleQuery(
	  tContext * __restrict aContext,
    unsigned * pqHandle
)
{
    aContext->particleGenerator.particleStorage.initParticleQuery(
        *((typename tContext::tParticleQuery*)pqHandle));
}

template<class tContext>
bool ShaderAPI<tContext>::getNextParticle(
	  tContext * __restrict aContext,
    unsigned * pqHandle,
    void  ** __restrict oParticle,
    float * __restrict posX,
    float * __restrict posY,
    float * __restrict posZ,
    float * __restrict intensity
)
{
    typename tContext::tParticle* ptrParticle;
    typename tContext::tParticleQuery particleQuery = *pqHandle;

    bool retval = aContext->particleGenerator.particleStorage.getNextParticle(
        particleQuery,
        ptrParticle);

    *oParticle = (void*)ptrParticle;
    *pqHandle = particleQuery;
    if(retval)
    {
      Vec3f1 pos= ptrParticle->getPosition();
      *posX = pos.x;
      *posY = pos.y;
      *posZ = pos.z;
      *intensity = ptrParticle->getIntensity().luminance();
    }

    return retval;
}

template<class tContext>
float ShaderAPI<tContext>::getParticleGeneratorClampThreshold(
	  tContext * __restrict aContext
)
{
    return aContext->particleGenerator.statistics.clampThreshold;
}

template<class tContext>
void ShaderAPI<tContext>::incrementTotalFrameLuminance(
	  tContext * __restrict aContext,
    float frameLuminanceContribution
)
{
    aContext->particleFilter.stats.incrementTotalFrameLuminance(frameLuminanceContribution);
}

//vectorized
template<class tContext>
void ShaderAPI<tContext>::trace_4(
	tContext       * __restrict aContext,
	const __m128i               mask,
	const __m128                aPositionX, //no eps added yet!
	const __m128                aPositionY, //no eps added yet!
	const __m128                aPositionZ, //no eps added yet!
	const __m128                aDirectionX,
	const __m128                aDirectionY,
	const __m128                aDirectionZ,
	const unsigned              aDepth, //not incremented yet!
	__m128         * __restrict oResultX,
	__m128         * __restrict oResultY,
	__m128         * __restrict oResultZ
)
{
	typedef typename Float4::Container t_FPacketC4;

	ActiveMask<4> rtfMask = *(ActiveMask<4>*)&mask;
	rtfMask.updateActiveContainers(); //necessary?

	if (rtfMask.isFalse()) return;

	RayPacket<4> rays;
	rays.org.x = *(const RTfact::Packet<4, float>*)&aPositionX;
	rays.org.y = *(const RTfact::Packet<4, float>*)&aPositionY;
	rays.org.z = *(const RTfact::Packet<4, float>*)&aPositionZ;
	rays.dir.x = *(const RTfact::Packet<4, float>*)&aDirectionX;
	rays.dir.y = *(const RTfact::Packet<4, float>*)&aDirectionY;
	rays.dir.z = *(const RTfact::Packet<4, float>*)&aDirectionZ;
	rays.org += rays.dir * t_FPacketC4::C_RAY_EPS; //add epsilon to prevent precision-issues

	rays.invDir = rays.dir.reciprocal();
	rays.tMin = t_FPacketC4::C_RAY_EPS;
	rays.tMax = t_FPacketC4::C_INFINITY;
	rays.depth = aDepth+1;

	RTfact::Vec3f<4> tmpResult;
	aContext->template trace<false, false, 4>(rays, rtfMask, tmpResult);
	*oResultX = *(__m128*)&tmpResult.x;
	*oResultY = *(__m128*)&tmpResult.y;
	*oResultZ = *(__m128*)&tmpResult.z;
}

template<class tContext>
void ShaderAPI<tContext>::getLC_4(
	tContext      * __restrict aContext,
	const __m128i              mask,
	const __m128               aPositionX,
	const __m128               aPositionY,
	const __m128               aPositionZ,
	const __m128               aDirectionX, //not negated yet!
	const __m128               aDirectionY, //not negated yet!
	const __m128               aDirectionZ, //not negated yet!
	const __m128               aDistance,
	__m128i       * __restrict oContributes, //not yet set to 0!
	__m128        * __restrict oIntensityX,
	__m128        * __restrict oIntensityY,
	__m128        * __restrict oIntensityZ
)
{
	typedef typename Float4::Container t_FPacketC4;

	ActiveMask<4> rtfMask = *(ActiveMask<4>*)&mask;
	rtfMask.updateActiveContainers(); //necessary?

	if (rtfMask.isFalse()) return;

	//prepare
	RayPacket<4> rays;
	rays.org.x = *(const RTfact::Packet<4, float>*)&aPositionX;
	rays.org.y = *(const RTfact::Packet<4, float>*)&aPositionY;
	rays.org.z = *(const RTfact::Packet<4, float>*)&aPositionZ;
	rays.dir.x = *(const RTfact::Packet<4, float>*)&aDirectionX;
	rays.dir.y = *(const RTfact::Packet<4, float>*)&aDirectionY;
	rays.dir.z = *(const RTfact::Packet<4, float>*)&aDirectionZ;
	rays.dir = -rays.dir;
	rays.invDir = rays.dir.reciprocal();
	rays.tMax = *(const RTfact::Packet<4, float>*)&aDistance - t_FPacketC4::C_RAY_EPS;
	rays.tMin = t_FPacketC4::C_RAY_EPS;

	ActiveMask<4> oMask;
	oMask.setFalseAndUpdateActiveContainers();

	RTfact::Vec3f<4> tmpIntensity;
	tmpIntensity.x = *(RTfact::Packet<4, float>*)oIntensityX;
	tmpIntensity.y = *(RTfact::Packet<4, float>*)oIntensityY;
	tmpIntensity.z = *(RTfact::Packet<4, float>*)oIntensityZ;
	aContext->template getLightContribution<false, 4>(rays, rtfMask, oMask, tmpIntensity);
	//*oContributes = *(__m128i*)&oMask;
	*(__m128*)oContributes = *(__m128*)&oMask;
	*oIntensityX = *(__m128*)&tmpIntensity.x;
	*oIntensityY = *(__m128*)&tmpIntensity.y;
	*oIntensityZ = *(__m128*)&tmpIntensity.z;
}

template<class tContext>
void ShaderAPI<tContext>::sampleLight_4(
	tContext       * __restrict aContext,
	const __m128i               mask,
	const unsigned              aLightIndex,
	const __m128                aPointX,
	const __m128                aPointY,
	const __m128                aPointZ,
	__m128         * __restrict oDirectionX,
	__m128         * __restrict oDirectionY,
	__m128         * __restrict oDirectionZ,
	__m128         * __restrict oDistance,
	__m128         * __restrict oRadianceX,
	__m128         * __restrict oRadianceY,
	__m128         * __restrict oRadianceZ,
	__m128         * __restrict oPositionX,
	__m128         * __restrict oPositionY,
	__m128         * __restrict oPositionZ
)
{
	ActiveMask<4> rtfMask = *(ActiveMask<4>*)&mask;
	rtfMask.updateActiveContainers(); //necessary?

	// extremely important, as aLightIndex might some bad value in case all rays
	// are masked out
	if (rtfMask.isFalse()) return;

	RTfact::Vec3f<4> point;
	point.x = *(const RTfact::Packet<4, float>*)&aPointX;
	point.y = *(const RTfact::Packet<4, float>*)&aPointY;
	point.z = *(const RTfact::Packet<4, float>*)&aPointZ;
	RTfact::Vec3f<4> tmpDirection;
	RTfact::Vec3f<4> tmpRadiance;
	RTfact::Vec3f<1> tmpPosition;
	RTfact::Packet<4, float> tmpDistance;
	
	/*aContext->scene.lights[aLightIndex]->template sample<4>(
		point,
		tmpDirection,
		tmpDistance,
		tmpRadiance,
		rtfMask, //*(ActiveMask<4>*)&mask,
		&tmpPosition
	);*/
	aContext->scene.lights[aLightIndex]->template illuminatePoint<4>(
        Math::uniformFloat2DC(),
		point,
		rtfMask,
		tmpDirection,
		tmpDistance,
		tmpRadiance,
		&tmpPosition
	);

	*oDirectionX = *(__m128*)&tmpDirection.x;
	*oDirectionY = *(__m128*)&tmpDirection.y;
	*oDirectionZ = *(__m128*)&tmpDirection.z;
	*oDistance = *(__m128*)&tmpDistance;
	*oRadianceX = *(__m128*)&tmpRadiance.x;
	*oRadianceY = *(__m128*)&tmpRadiance.y;
	*oRadianceZ = *(__m128*)&tmpRadiance.z;
	*oPositionX = _mm_set_ps1(tmpPosition.x);
	*oPositionY = _mm_set_ps1(tmpPosition.y);
	*oPositionZ = _mm_set_ps1(tmpPosition.z);
}

template<class tContext>
void ShaderAPI<tContext>::sampleDT_4(
		tContext      * __restrict aContext,
		const __m128i              mask,
		const void    * __restrict aTextureHandle,
		const __m128               aTexCoordX,
		const __m128               aTexCoordY,
		const __m128               aTexCoordOffsetX,
		const __m128               aTexCoordOffsetY,
		__m128        * __restrict oResultX,
		__m128        * __restrict oResultY,
		__m128        * __restrict oResultZ
)
{
	ActiveMask<4> rtfMask = *(ActiveMask<4>*)&mask;
	rtfMask.updateActiveContainers(); //necessary?

	if (rtfMask.isFalse()) return;
#ifndef ANYSL_RTFACT_BYTE_AND_FLOAT_TEXTURE_SUPPORT
    const BasicImage2D<float>* image = reinterpret_cast<const BasicImage2D<float>*>(aTextureHandle);

	RTfact::Vec3f<4> tmpResult;
    sampleTexture<true, false>(
		image,
		*(RTfact::Packet<4, float>*)&aTexCoordX,
		*(RTfact::Packet<4, float>*)&aTexCoordY,
		rtfMask, //*(ActiveMask<4>*)&mask,
		&tmpResult
	);
#else
    const RTImageImpl* image = reinterpret_cast<const RTImageImpl*>(aTextureHandle);

    RTfact::Vec3f<4> tmpResult;

    sampleRTImageImpl(
        image,
        *((RTfact::Packet<4, float>*) &aTexCoordX),
        *((RTfact::Packet<4, float>*) &aTexCoordY),
        rtfMask,
        tmpResult);
#endif

	*oResultX = *(__m128*)&tmpResult.x;
	*oResultY = *(__m128*)&tmpResult.y;
	*oResultZ = *(__m128*)&tmpResult.z;
}

template<class tContext>
void ShaderAPI<tContext>::sampleDT_id_4(
	tContext       * __restrict aContext,
	const __m128i               mask,
	void           * __restrict aTextureTable,
	const unsigned              aTextureID,
	const __m128                aTexCoordX,
	const __m128                aTexCoordY,
	const __m128                aTexCoordOffsetX,
	const __m128                aTexCoordOffsetY,
	__m128         * __restrict oResultX,
	__m128         * __restrict oResultY,
	__m128         * __restrict oResultZ
)
{
	// This function only wraps around sampleDT_4

	// In aTextureTable pointer to the texture data
	// table is stored. Texture table is an array of void *,
    // where each element points to internal texture data.
	if (aTextureTable) {
	    sampleDT_4(aContext,
				   mask,
	               reinterpret_cast<void**>(aTextureTable)[aTextureID],
	               aTexCoordX,
	               aTexCoordY,
	               aTexCoordOffsetX,
	               aTexCoordOffsetY,
	               oResultX, oResultY, oResultZ);
	} else {
		*oResultX = _mm_set_ps1(0.f);
		*oResultY = _mm_set_ps1(1.f);
		*oResultZ = _mm_set_ps1(0.f);
	}
}



RTFACT_REMOTE_NAMESPACE_END


#endif


