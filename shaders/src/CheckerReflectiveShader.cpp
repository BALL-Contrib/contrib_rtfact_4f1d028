/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.345 */
/* Listing 16.12  Surface shader providing checkerboard pattern */

/*
 * checker(): surface shader for applying a checkerboard pattern.
 */

#include "RTfactShader.hpp"

PARAMETER float Kd;
PARAMETER float Ka;
PARAMETER float frequency;
PARAMETER float s_scale;
PARAMETER float t_scale;
PARAMETER Color blackcolor;
PARAMETER Color whitecolor;
PARAMETER float reflectance;
PARAMETER float maxDepth;

BEGIN_INIT_PARAM
INIT_PARAM(Kd, 0.5f)
INIT_PARAM(Ka, 0.1f)
INIT_PARAM(frequency, 4.0f)
INIT_PARAM(s_scale, 1.0f)
INIT_PARAM(t_scale, 1.0f)
INIT_PARAM(blackcolor, Color(0.0f, 0.0f, 0.0f))
INIT_PARAM(whitecolor, Color(1.0f, 1.0f, 1.0f))
INIT_PARAM(reflectance, 0.5f)
INIT_PARAM(maxDepth, 2.f)
END_INIT_PARAM

SURFACE_SHADER
{
    const Point P = origin + hitDistance * dir;
    const Vector & IN = dir;  /* normalized incident vector */
    const Normal & N = normal;  /* surface normal */

    Normal Nf = FaceForward(N, IN);

    float smod = Mod (texCoord1 * s_scale * frequency, 1.0f);
    float tmod = Mod (texCoord2 * t_scale * frequency, 1.0f);

    Color Ci;
	
    if (smod < 0.5f)
    {
	if (tmod < 0.5f) Ci = whitecolor;
	else Ci = blackcolor;
    }
    else
    {
	if (tmod < 0.5f) Ci = blackcolor;
	else Ci = whitecolor;
    }
	
    //Color C_diffuse(0.5f, 0.5f, 0.5f);
    Color C_diffuse(0.0f, 0.0f, 0.0f);
    BEGIN_ILLUMINANCE_LOOP(P) {
        C_diffuse += diffuseComponent(L_dir_norm, P, Nf, Cl);
    } END_ILLUMINANCE_LOOP;

    Point reflDir = Reflect(dir, Nf);

    if (depth < maxDepth) {
        //TraceRay(P, reflDir, result); //causes problems...
		Color Crefl;
        TraceRay(P, reflDir, Crefl);
        result = (Ci * (Ka + Kd * C_diffuse)) * (1.f-reflectance) + Crefl * reflectance;
    } else {
        result = Ci * (Ka + Kd  * C_diffuse);
    }
}

