#ifndef _INTERNAL_API_HPP
#define _INTERNAL_API_HPP

//############################ Scalar API ############################

extern "C" void API_trace_1 (
	void  * context,
	Point   origin,
	Point   dir,
	int     depth,
	Color & oColor
);

extern "C" void API_getLC_1 (
	void  * context,
	Point   origin,
	Point   dir,
	float   max,
	int   & contributes,
	Color & oIntensity
);

extern "C" int API_getNumLight (
	void  * context
);

extern "C" void API_sampleLight_1 (
	void     * context,
	unsigned   light,
	Point      aPoint,
	Point    & oDirection,
	float    & oDistance,
	Color    & oRadiance,
	Point    & oPosition
);

//samples a diffuse texture source
extern "C" void API_sampleDT_1 (
	void  * context,
	void  * handle,
	float   texCoordX,
	float   texCoordY,
	float   texCoordOffsetX,
	float   texCoordOffsetY,
	Color & oResult
);

extern "C" float API_getTime(void* context);

//############################### SSE API #########################

extern "C" void API_trace_4 (
	void            * context,
	t_Packed_Mask4    mask,

	t_Packed_Float4   originX,
	t_Packed_Float4   originY,
	t_Packed_Float4   originZ,

	t_Packed_Float4   dirX,
	t_Packed_Float4   dirY,
	t_Packed_Float4   dirZ,

	t_Packed_Int4     depth,
	t_Packed_Vec3f4 & oColor
);

extern "C" void API_getLC_4 (
	void            * context,
	t_Packed_Mask4    mask,

	t_Packed_Float4   originX,
	t_Packed_Float4   originY,
	t_Packed_Float4   originZ,

	t_Packed_Float4   dirX,
	t_Packed_Float4   dirY,
	t_Packed_Float4   dirZ,

	t_Packed_Float4   max,

	t_Packed_Int4   & contributes,
	t_Packed_Vec3f4 & oIntensity
);

extern "C" void API_sampleLight_4 (
	void            * context,
	t_Packed_Mask4    mask,
	t_Packed_Int4     light,

	t_Packed_Float4   aPointX,
	t_Packed_Float4   aPointY,
	t_Packed_Float4   aPointZ,

	t_Packed_Vec3f4 & oDirection,
	t_Packed_Float4 & oDistance,
	t_Packed_Vec3f4 & oRadiance,
	t_Packed_Vec3f4 & oPosition
);
extern "C" void API_sampleDT_4 (
    void            * context,
    t_Packed_Mask4    mask,
    void            * handle,
    t_Packed_Float4   texCoordX,
    t_Packed_Float4   texCoordY,
    t_Packed_Float4   texCoordOffsetX,
    t_Packed_Float4   texCoordOffsetY,
    t_Packed_Vec3f4 & oResult
);

#endif
