#ifndef JITRT_RTFACT_SHADER_HPP_
#define JITRT_RTFACT_SHADER_HPP_


//#include "commontypes.hpp"
//#include "internalapi.hpp"

//#define _ALIGN __attribute__((force_align_arg_pointer))
//#define _NOINLINE __attribute__((noinline))

//###################################################################### RTfact API #############################################
extern "C" void RTFACT_trace_1
(
	void           * __restrict aContext,
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
);

extern "C" void RTFACT_traceParticle_1
(
	void           * __restrict aContext,
	const float                 aPositionX,
	const float                 aPositionY,
	const float                 aPositionZ,
	const float                 aDirectionX,
	const float                 aDirectionY,
	const float                 aDirectionZ,
	const float                 aCumulativeFactorX,
	const float                 aCumulativeFactorY,
	const float                 aCumulativeFactorZ,
  const int                   aPrimeBase,
  const int                   aRandomSeed,
  const bool                  aInside,
	const unsigned              aDepth,
	float          * __restrict oResultX,
	float          * __restrict oResultY,
	float          * __restrict oResultZ
);

extern "C" void RTFACT_getLC_1
(
	void        * __restrict aContext,
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
);

extern "C" void RTFACT_sampleLight_1
(
	void           * __restrict aContext,
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
);

extern "C" bool RTFACT_getLightIntensity_1
(
    const float aPointX,
    const float aPointY,
    const float aPointZ,
    const float aNormalX,
    const float aNormalY,
    const float aNormalZ,
    const void * __restrict light,
    void * __restrict context,
    float * __restrict oDirToLightX,
    float * __restrict oDirToLightY,
    float * __restrict oDirToLightZ,
	  float * __restrict oIntensityX,
    float * __restrict oIntensityY,
    float * __restrict oIntensityZ,
	  float * __restrict oDotLightNormal,
    bool * __restrict oLightMask
);

extern "C" bool RTFACT_getLightSourceIntensity_1
(
    void * __restrict context,
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
);

extern "C" void RTFACT_sampleDT_1
(
	void        * __restrict aContext,
	const void  * __restrict aTextureHandle,
	const float              aTexCoordX,
	const float              aTexCoordY,
	const float              aTexCoordOffsetX,
	const float              aTexCoordOffsetY,
	float       * __restrict oResultX,
	float       * __restrict oResultY,
	float       * __restrict oResultZ
);

extern "C" void RTFACT_sampleDT_1
(
	void        * __restrict aContext,
	const void  * __restrict aTextureHandle,
	const float              aTexCoordX,
	const float              aTexCoordY,
	const float              aTexCoordOffsetX,
	const float              aTexCoordOffsetY,
	float       * __restrict oResultX,
	float       * __restrict oResultY,
	float       * __restrict oResultZ
);

extern "C" unsigned RTFACT_getNumLight
(
	void * __restrict aContext
);

extern "C" unsigned RTFACT_getLightPathCount
(
	void * __restrict aContext
);

extern "C" float RTFACT_getTime
(
	void * __restrict aContext
);

extern "C" bool RTFACT_getParticlesGenerated
(
	void * __restrict aContext
);

extern "C" void RTFACT_insertNewImporton
(
	void * __restrict aContext,
  const float              hitX,
  const float              hitY,
  const float              hitZ,
  const float              intensityX,
  const float              intensityY,
  const float              intensityZ,
  const float              normalX,
  const float              normalY,
  const float              normalZ
);

extern "C" void RTFACT_assessParticle(
	void * __restrict       aContext,
  const float             hitX,
  const float             hitY,
  const float             hitZ,
  const float             intensityX,
  const float             intensityY,
  const float             intensityZ,
  const float             normalX,
  const float             normalY,
  const float             normalZ
);

extern "C" void RTFACT_initParticleQuery(
    void  * __restrict context,
    unsigned * pqHandle
);

extern "C" bool RTFACT_getNextParticle(
    void  * __restrict context,
    unsigned * pqHandle,
    void  ** __restrict particle,
    float * __restrict posX,
    float * __restrict posY,
    float * __restrict posZ,
    float * __restrict intensity
);

extern "C" float RTFACT_getParticleGeneratorClampThreshold(
    void  * __restrict context
);

extern "C"  void RTFACT_incrementTotalFrameLuminance(
    void  * __restrict context,
    float frameLuminanceContribution
);




/* CPP - bindings */

struct Context {
	static inline void trace(
			void        * __restrict context,
			const Point         origin,
			const Point         dir,
			const unsigned      depth,
			Color       & oColor
	);

	static inline void traceParticle(
			void        * __restrict context,
			const Point         origin,
			const Point         dir,
		  const Color         cumulativeFactor,
      const int           primeBase,
      const int           randomSeed,
      const bool          inside,
			const unsigned      depth,
			Color       & oColor
	);

	static inline unsigned getLightContribution(
			void        * __restrict context,
			const Point         origin,
			const Point         dir,
			const float         max,
			Color       & oIntensity
	);

	static inline unsigned getNumLightSources(
			void        * __restrict context
	);

  static inline unsigned getLightPathCount(
      void        * __restrict context
	);

	static inline void sampleLightSource(
			void        * __restrict context,
			const unsigned      light,
			const Point 	    aPoint,
			Point       & oDirection,
			float       & oDistance,
			Color       & oRadiance,
			Point       & oPosition
	);

  static inline bool getLightIntensity(
      const Point       aPoint,
      const Normal      aNormal,
			const Light       light,
			void * __restrict context,
      Vector&           oDirToLight,
      Vector&           oIntensity,
      float&            oDotLightNormal,
      bool&             oLightMask);

  static inline bool getLightSourceIntensity(
			void * __restrict context,
      const int         aLightIndex,
      const Point       aPoint,
      const Normal      aNormal,
      Vector&           oDirToLight,
      Vector&           oIntensity,
      float&            oDotLightNormal);

	//samples a diffuse texture source
	static inline Color sampleDiffuseTexture(
			void  * __restrict context,
			const void  * __restrict handle,
			const float   texCoordX,
			const float   texCoordY,
			const float   texCoordOffsetX,
			const float   texCoordOffsetY
	);

  static inline float getTime(
          void * __restrict context
  );

  static inline bool getParticlesGenerated(
    void  * __restrict context
  );

  static inline void insertNewImporton(
      void  * __restrict context,
      const Point & hit,
      const Color & intensity,
      const Normal & normal
  );

  static inline void assessParticle(
      void  * __restrict context,
      const Point & hit,
      const Color & intensity,
      const Normal & normal
  );

  static inline void initParticleQuery(
      void  * __restrict context,
      unsigned * pqHandle
  );

  static inline bool getNextParticle(
      void  * __restrict context,
      unsigned & pqHandle,
      Particle & particle,
      Point & position,
      float & intensity
  );

  static inline float getParticleGeneratorClampThreshold(
      void  * __restrict context
  );

  static inline void incrementTotalFrameLuminance(
      void  * __restrict context,
      float frameLuminanceContribution
  );

};

void Context::trace(
		void  * __restrict context,
		const Point   origin,
		const Point   dir,
		const unsigned     depth,
		Color & oColor
)
{
	float tmpResultX, tmpResultY, tmpResultZ;
	RTFACT_trace_1(
		context,
		origin.x, origin.y, origin.z, //RTFACT_trace_1 adds EPS
		dir.x, dir.y, dir.z,
		depth,  //RTFACT_trace_1 does the increment
		&tmpResultX, &tmpResultY, &tmpResultZ
	);
	oColor.x = tmpResultX;
	oColor.y = tmpResultY;
	oColor.z = tmpResultZ;
}

void Context::traceParticle(
		void  * __restrict context,
		const Point   origin,
		const Point   dir,
		const Color   cumulativeFactor,
    const int     primeBase,
    const int     randomSeed,
    const bool    inside,
		const unsigned     depth,
		Color & oColor
)
{
	float tmpResultX, tmpResultY, tmpResultZ;
	RTFACT_traceParticle_1(
		context,
		origin.x, origin.y, origin.z, //RTFACT_trace_1 adds EPS
		dir.x, dir.y, dir.z,
    cumulativeFactor.x, cumulativeFactor.y, cumulativeFactor.z,
    primeBase, randomSeed, inside,
		depth,  //RTFACT_trace_1 does the increment
		&tmpResultX, &tmpResultY, &tmpResultZ
	);
	oColor.x = tmpResultX;
	oColor.y = tmpResultY;
	oColor.z = tmpResultZ;
}


unsigned Context::getLightContribution(
		void  * __restrict context,
		const Point   origin,
		const Point   dir,
		const float   max,
		Color & oIntensity
)
{
	unsigned contributes;
	float tmpIntensityX = oIntensity.x;
	float tmpIntensityY = oIntensity.y;
	float tmpIntensityZ = oIntensity.z;
	RTFACT_getLC_1(
		context,
		origin.x, origin.y, origin.z,
		dir.x, dir.y, dir.z,  //RTFACT_getLC_1 does the negation
		max,
		&contributes, //RTFACT_getLC_1 does the setting to 0
		&tmpIntensityZ, &tmpIntensityY, &tmpIntensityZ
	);
	oIntensity.x = tmpIntensityX;
	oIntensity.y = tmpIntensityY;
	oIntensity.z = tmpIntensityZ;
	return contributes;
}

void Context::sampleLightSource(
		void * __restrict context,
		const unsigned      light,
		const Point 		  aPoint,
		Point       & oDirection,
		float       & oDistance,
		Color       & oRadiance,
		Point       & oPosition
)
{
	float tmpDirectionX, tmpDirectionY, tmpDirectionZ;
	float tmpRadianceX, tmpRadianceY, tmpRadianceZ;
	float tmpPositionX, tmpPositionY, tmpPositionZ;
	RTFACT_sampleLight_1(
		context,
        light,
		aPoint.x, aPoint.y, aPoint.z,
		&tmpDirectionX, &tmpDirectionY, &tmpDirectionZ,
		&oDistance,
		&tmpRadianceX, &tmpRadianceY, &tmpRadianceZ,
		&tmpPositionX, &tmpPositionY, &tmpPositionZ
	);
	oDirection.x = tmpDirectionX;
	oDirection.y = tmpDirectionY;
	oDirection.z = tmpDirectionZ;
	oRadiance.x = tmpRadianceX;
	oRadiance.y = tmpRadianceY;
	oRadiance.z = tmpRadianceZ;
	oPosition.x = tmpPositionX;
	oPosition.y = tmpPositionY;
	oPosition.z = tmpPositionZ;
}

bool Context::getLightIntensity(
    const Point       aPoint,
    const Normal      aNormal,
		const Light       light,
		void * __restrict context,
    Vector&           oDirToLight,
    Vector&           oIntensity,
    float&            oDotLightNormal,
    bool&             oLightMask
)
{
  return RTFACT_getLightIntensity_1(
    aPoint.x, aPoint.y, aPoint.z,
    aNormal.x, aNormal.y, aNormal.z,
    light,
    context,
    &(oDirToLight.x), &(oDirToLight.y), &(oDirToLight.z),
	  &(oIntensity.x), &(oIntensity.y), &(oIntensity.z),
	  &oDotLightNormal, &oLightMask);
}

bool Context::getLightSourceIntensity(
		void * __restrict context,
    const int         aLightIndex,
    const Point       aPoint,
    const Normal      aNormal,
    Vector&           oDirToLight,
    Vector&           oIntensity,
    float&            oDotLightNormal)
{
  return RTFACT_getLightSourceIntensity_1(
    context,
    aLightIndex,
    aPoint.x, aPoint.y, aPoint.z,
    aNormal.x, aNormal.y, aNormal.z,
    &(oDirToLight.x), &(oDirToLight.y), &(oDirToLight.z),
	  &(oIntensity.x), &(oIntensity.y), &(oIntensity.z),
	  &oDotLightNormal);
}

//samples a diffuse texture source
Color Context::sampleDiffuseTexture(
		void  * __restrict context,
		const void  * __restrict handle,
		const float   texCoordX,
		const float   texCoordY,
		const float   texCoordOffsetX,
		const float   texCoordOffsetY
)
{
	float tmpResultX, tmpResultY, tmpResultZ;
    RTFACT_sampleDT_1(
        context,
        handle,
        texCoordX,
        texCoordY,
        texCoordOffsetX,
        texCoordOffsetY,
		&tmpResultX, &tmpResultY, &tmpResultZ
    );
	return Color(tmpResultX, tmpResultY, tmpResultZ);
}

unsigned Context::getNumLightSources(void * __restrict context) {
	return RTFACT_getNumLight(context);
}

unsigned Context::getLightPathCount(void * __restrict context) {
	return RTFACT_getLightPathCount(context);
}

float Context::getTime(void * __restrict context) {
    return RTFACT_getTime(context);
}

bool Context::getParticlesGenerated(
    void  * __restrict context)
{
    return RTFACT_getParticlesGenerated(context);
}

void Context::insertNewImporton(
    void  * __restrict context,
    const Point & hit,
    const Color & intensity,
    const Normal & normal
)
{
	  RTFACT_insertNewImporton(context,
        hit.x, hit.y, hit.z,
        intensity.x, intensity.y, intensity.z,
        normal.x, normal.y, normal.z);
}

void Context::assessParticle(
    void  * __restrict context,
    const Point & hit,
    const Color & intensity,
    const Normal & normal
)
{
	  RTFACT_assessParticle(context,
        hit.x, hit.y, hit.z,
        intensity.x, intensity.y, intensity.z,
        normal.x, normal.y, normal.z);
}

void Context::initParticleQuery(
    void  * __restrict context,
    unsigned * pqHandle
)
{
	  RTFACT_initParticleQuery(context, pqHandle);
}

bool Context::getNextParticle(
    void  * __restrict context,
    unsigned & pqHandle,
    Particle & particle,
    Point & pos,
    float & intensity
)
{
	  return RTFACT_getNextParticle(
      context, &pqHandle, (void**)&particle,
      &(pos.x), &(pos.y), &(pos.z),
      &intensity);
}

float Context::getParticleGeneratorClampThreshold(
    void  * __restrict context
)
{
	  return RTFACT_getParticleGeneratorClampThreshold(context);
}

void Context::incrementTotalFrameLuminance(
    void  * __restrict context,
    float frameLuminanceContribution
)
{
	  return RTFACT_incrementTotalFrameLuminance(context, frameLuminanceContribution);
}




extern "C" void shade
(
        void * __restrict  context,
        const unsigned     rayType,
        const bool         inside,
        const int          primeBase,
        const float        cumulativeFactorX,
        const float        cumulativeFactorY,
        const float        cumulativeFactorZ,
        const int          randomSeed,
        const unsigned     maxDepth,
        const unsigned     depth,
        const float        originX,
        const float        originY,
        const float        originZ,
        const float        dirX,
        const float        dirY,
        const float        dirZ,
        const unsigned     appearanceID,
        const float        hitDistance,
        const float        geomNormalX,
        const float        geomNormalY,
        const float        geomNormalZ,
        const float        normalX,
        const float        normalY,
        const float        normalZ,
        const float        vertColorX,
        const float        vertColorY,
        const float        vertColorZ,
        const float        u,
        const float        v,
        const float        texCoord1,
        const float        texCoord2,
        float * __restrict oResultX,
        float * __restrict oResultY,
        float * __restrict oResultZ,
        const float        tangentX,
        const float        tangentY,
        const float        tangentZ,
        const float        binormalX,
        const float        binormalY,
        const float        binormalZ
);


// packetization source (masked wrapper for shader)
extern "C" void masked_shade
(
        void * __restrict  context,
        const unsigned     rayType,
        const bool         inside,
        const int          primeBase,
        const float        cumulativeFactorX,
        const float        cumulativeFactorY,
        const float        cumulativeFactorZ,
        const int          randomSeed,
        const unsigned     maxDepth,
        const unsigned     depth,
        const unsigned     activeMask,
        const float        originX,
        const float        originY,
        const float        originZ,
        const float        dirX,
        const float        dirY,
        const float        dirZ,
        const unsigned     appearanceID,
        const float        hitDistance,
        const float        geomNormalX,
        const float        geomNormalY,
        const float        geomNormalZ,
        const float        normalX,
        const float        normalY,
        const float        normalZ,
        const float        vertColorX,
        const float        vertColorY,
        const float        vertColorZ,
        const float        u,
        const float        v,
        const float        texCoord1,
        const float        texCoord2,
        float * __restrict oResultX,
        float * __restrict oResultY,
        float * __restrict oResultZ,
        const float        tangentX,
        const float        tangentY,
        const float        tangentZ,
        const float        binormalX,
        const float        binormalY,
        const float        binormalZ
)
{
	if (activeMask != 0) {
    //printf("BEFORE SHADE\n");
    //printf("binormalZ = %f\n", binormalZ);
		shade
		(
            context,
            rayType,
            inside,
            primeBase,
            cumulativeFactorX,
            cumulativeFactorY,
            cumulativeFactorZ,
            randomSeed,
            maxDepth,
            depth,
            originX,
            originY,
            originZ,
            dirX,
            dirY,
            dirZ,
            appearanceID,
            hitDistance,
            geomNormalX,
            geomNormalY,
            geomNormalZ,
            normalX,
            normalY,
            normalZ,
            vertColorX,
            vertColorY,
            vertColorZ,
            u,
            v,
            texCoord1,
            texCoord2,
            oResultX,
            oResultY,
            oResultZ,
            tangentX,
            tangentY,
            tangentZ,
            binormalX,
            binormalY,
            binormalZ
		);
    //printf("AFTER SHADE\n");
	}
}


// packetized shade functions
// These are either resolved by automatic packetization or by linking the scalar wrapper module.
extern "C" void masked_shade_4
(
        void * __restrict   context,
        const unsigned      rayType,
        const bool          inside,
        const __m128i       primeBase,
        const __m128        cumulativeFactorX,
        const __m128        cumulativeFactorY,
        const __m128        cumulativeFactorZ,
        const __m128i       randomSeed,
        const unsigned      maxDepth,
        const unsigned      depth,
        const __m128i	    activeMask,
        const __m128         originX,
        const __m128         originY,
        const __m128         originZ,
        const __m128        dirX,
        const __m128        dirY,
        const __m128        dirZ,
        const __m128i       appearanceID,
        const __m128        hitDistance,
        const __m128        geomNormalX,
        const __m128        geomNormalY,
        const __m128        geomNormalZ,
        const __m128 	    normalX,
        const __m128 	    normalY,
        const __m128  	    normalZ,
        const __m128	    vertColorX,
        const __m128 	    vertColorY,
        const __m128        vertColorZ,
        const __m128        u,
        const __m128        v,
        const __m128        texCoord1,
        const __m128        texCoord2,
        __m128 * __restrict oResultX,
        __m128 * __restrict oResultY,
        __m128 * __restrict oResultZ,
        const __m128        tangentX,
        const __m128        tangentY,
        const __m128        tangentZ,
        const __m128        binormalX,
        const __m128        binormalY,
        const __m128        binormalZ
);

extern "C" void masked_shade_4_commonOrigin
(
        void * __restrict   context,
        const unsigned      rayType,
        const bool          inside,
        const __m128i       primeBase,
        const __m128        cumulativeFactorX,
        const __m128        cumulativeFactorY,
        const __m128        cumulativeFactorZ,
        const __m128i       randomSeed,
        const unsigned      maxDepth,
        const unsigned      depth,
        const __m128i	    activeMask,
        const float         originX,
        const float         originY,
        const float         originZ,
        const __m128        dirX,
        const __m128        dirY,
        const __m128        dirZ,
        const __m128i       appearanceID,
        const __m128        hitDistance,
        const __m128        geomNormalX,
        const __m128        geomNormalY,
        const __m128        geomNormalZ,
        const __m128 	    normalX,
        const __m128 	    normalY,
        const __m128  	    normalZ,
        const __m128	    vertColorX,
        const __m128 	    vertColorY,
        const __m128        vertColorZ,
        const __m128        u,
        const __m128        v,
        const __m128        texCoord1,
        const __m128        texCoord2,
        __m128 * __restrict oResultX,
        __m128 * __restrict oResultY,
        __m128 * __restrict oResultZ,
        const __m128        tangentX,
        const __m128        tangentY,
        const __m128        tangentZ,
        const __m128        binormalX,
        const __m128        binormalY,
        const __m128        binormalZ
);





// packetized API calls

extern "C" void RTFACT_trace_4(
	void           * __restrict aContext,
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
);

extern "C" void RTFACT_getLC_4(
	void          * __restrict aContext,
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
);

extern "C" void RTFACT_sampleLight_4(
	void           * __restrict aContext,
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
);

//samples a diffuse texture source
extern "C" void RTFACT_sampleDT_4(
    void          * __restrict aContext,
    const __m128i              mask,
    const void    * __restrict aTextureHandle,
    const __m128               aTexCoordX,
    const __m128               aTexCoordY,
    const __m128               aTexCoordOffsetX,
    const __m128               aTexCoordOffsetY,
    __m128        * __restrict oResultX,
    __m128        * __restrict oResultY,
    __m128        * __restrict oResultZ
);

extern "C" void RTFACT_sampleDT_id_4
(
	const void     * __restrict aContext,
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
);




// dummy function for fake calls
// required in order to prevent dead function removal of packetized API declarations
void ___fakeCall()
{
	void* voidptr;
	__m128 v;
	__m128* vp;
	__m128i vi;
	__m128i* vip;
	unsigned u;

	RTFACT_trace_4(
		voidptr,
		vi,
		v, v, v, v, v, v,
		u, vp, vp, vp);

	RTFACT_getLC_4(
		voidptr,
		vi,
        v, v, v, v, v, v, v,
        vip, vp, vp, vp);

	RTFACT_sampleLight_4(
        voidptr,
        vi,
        u,
        v, v, v,
        vp, vp, vp, vp, vp, vp, vp, vp, vp, vp);

	RTFACT_sampleDT_4(
			voidptr,
			vi,
			voidptr,
			v, v, v, v,
			vp, vp, vp);

	RTFACT_sampleDT_id_4(
			voidptr,
			vi,
			voidptr,
			u,
			v, v, v, v,
			vp, vp, vp);

  bool b;
	float f;
	masked_shade_4             (voidptr, u, b, vi, v, v, v, vi, u, u, vi, v, v, v, v, v, v, vi, v, v, v, v, v, v, v, v, v, v, v, v, v, v, vp, vp, vp, v, v, v, v, v, v);
	masked_shade_4_commonOrigin(voidptr, u, b, vi, v, v, v, vi, u, u, vi, f, f, f, v, v, v, vi, v, v, v, v, v, v, v, v, v, v, v, v, v, v, vp, vp, vp, v, v, v, v, v, v);
}



#endif /* JITRT_RTFACT_SHADER_HPP_ */
