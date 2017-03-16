#include "RTfactShader.hpp"

/**
 * From RenderMan Specification
 */

PARAMETER float eta;        /* index of refraction */
PARAMETER float Kr;         /* reflective coefficient */
PARAMETER float Kt;         /* transmissive coefficient */
PARAMETER float Ks;         /* specular coefficient */
PARAMETER float Kss;        /* specular exponent */
PARAMETER float Kd;
PARAMETER float maxDepth;

BEGIN_INIT_PARAM
INIT_PARAM_DESCR(eta, 1.5, "index of refraction")
INIT_PARAM_DESCR(Kr, 8, "reflective coefficient")
INIT_PARAM_DESCR(Kt, 2, "transmissive coefficient")
INIT_PARAM_DESCR(Ks, 2, "specular coefficient")
INIT_PARAM_DESCR(Kss, 2, "specular exponent")
INIT_PARAM(Kd, 5)
INIT_PARAM(maxDepth, 2)
END_INIT_PARAM

SURFACE_SHADER
{
    const Point P = origin + hitDistance * dir;
    const Vector & IN = dir; /* normalized incident vector */
    const Normal & N = normal;  /* surface normal */

    Normal Nn = FaceForward(N, IN);

    /* ambient term */
    Color Ci = Kd; /* * ambient(); */
        
    /* diffuse and specular terms */
    BEGIN_ILLUMINANCE_LOOP(P) {
        /* diffuse */
        Ci += Kd * Cl * Dot(L_dir, Nn);
        /* specular */
        Vector H = Normalized(L_dir_norm+IN);
        Ci += Ks * Cl * Pow(Max(0.0f, Dot(Nn, H)), Kss);
    } END_ILLUMINANCE_LOOP;


    Color rayColor;
    /* reflection */
    Vector R = Reflect( IN, Nn );

    if (depth < maxDepth && Ks > 0.0f)
    {
        //TraceRay(P + RAY_EPS * R, R, rayColor);
        TraceRay(P, R, rayColor);
        Ci += Ks * rayColor;
    }

    /* transmittance */
    if (Kt > 0.0f)
    {
        Vector T = Refract( IN, Nn, Dot(N, IN) < 0.0f ? eta : 1.0f/eta );
        if ( Length(T) != 0.0f && depth < maxDepth)
        {
            //TraceRay(P + RAY_EPS * T, T, rayColor);
            TraceRay(P, T, rayColor);
            Ci += Kt * rayColor;
        }
    }
    
    result = Ci;
}
