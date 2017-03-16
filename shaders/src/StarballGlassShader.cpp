/************************************************************************
 * starball.sl - make a striped ball with a star pattern
 *
 * Reference:
 *   _Advanced RenderMan: Creating CGI for Motion Picture_, 
 *   by Anthony A. Apodaca and Larry Gritz, Morgan Kaufmann, 1999.
 ************************************************************************/
/*
 * glass.sl -- Shiny reflective & refractive glass, using ray tracing.
 *
 * DESCRIPTION:
 *   Makes semi-transparent glass, using ray tracing to calculate
 *   reflections and refractions of the environment.
 * 
 * PARAMETERS:
 *    Ka, Kd, Ks, roughness, specularcolor - The usual meaning
 *    Kr - coefficient for mirror-like reflections of environment
 *    Kt - coefficient for refracted transmission
 *    transmitcolor - color of the glass
 *    eta - the coefficient of refraction of the glass
 *    blur - how blurry are the reflections/refractions? (0 = perfectly sharp)
 *    samples - set to higher than 1 for oversampling of blur
 *
 * AUTHOR: written by Larry Gritz, 1991
 *
 * HISTORY:
 *
 * $Revision: 1.1.1.1 $      $Date: 2002/02/10 02:35:49 $
 *
 * $Log: glass.sl,v $
 * Revision 1.1.1.1  2002/02/10 02:35:49  tal
 * RenderMan Repository
 *
 * Revision 1.4.2.2  1998-02-06 14:02:30-08  lg
 * Converted to "modern" SL with appropriate use of vector & normal types
 *
 * Revision 1.4.2.1  1998-01-05 21:05:56-08  lg
 * Improvement to stratify solid angle ray sampling
 *
 * Revision 1.4  1996-03-11 19:04:51-08  lg
 * Don't use Os -- we just want to set the color from the rays
 *
 * Revision 1.3  1996-02-29 10:11:23-08  lg
 * Minor correction and dead code removal
 *
 * Revision 1.2  1996-02-12 17:34:53-08  lg
 * Fixed use of fresnel, chose better default parameters
 *
 * Revision 1.1  1995-12-05 15:05:47-08  lg
 * Initial RCS-protected revision
 *
 * 25 Jan 1994 -- recoded by lg in correct shading language.
 * Aug 1991 -- written by lg in C
 *
 */

#include "RTfactShader.hpp"
#include "Material.hpp"

PARAMETER float Ka;
PARAMETER float Kd;
PARAMETER float Ks;
PARAMETER float Kr;       /* reflective coefficient */
PARAMETER float Kt;       /* transmissive coefficient */
PARAMETER float roughness;
PARAMETER float blur;
PARAMETER float eta;      /* index of refraction */
PARAMETER Color specularcolor;
PARAMETER Color transmitcolor;
PARAMETER float samples;

PARAMETER float maxDepth;

BEGIN_INIT_PARAM
INIT_PARAM(Ka, 1.0f)
INIT_PARAM(Kd, 0.5f)
INIT_PARAM(Ks, 0.5f)
INIT_PARAM_DESCR(Kr, 1.0f, "reflective coefficient")
INIT_PARAM_DESCR(Kt, 1.0f, "transmissive coefficient")
INIT_PARAM(roughness, 0.1f)
INIT_PARAM(blur, 0.0f)
INIT_PARAM_DESCR(eta, 1.5f, "index of refraction")
INIT_PARAM(specularcolor, Color(1.0f,1.0f,1.0f))
INIT_PARAM(transmitcolor, Color(1.0f,1.0f,1.0f))
INIT_PARAM(samples, 1.0f)
INIT_PARAM(maxDepth, 2.f)
END_INIT_PARAM

SURFACE_SHADER
{
	const Point P = origin + hitDistance * dir;
    const Vector & IN = dir;  /* normalized incident vector */
    const Normal & N = normal;  /* surface normal */

    // starball

    const float dv = 0.005f; // dv not supported yet
    const float du = 0.005f; // du not supported yet
    float ddv = 2.0f * Abs(dv);
    float ddu = 2.0f * Abs(du);

    float ang = Mod(texCoord1*360.0f, 144.0f);
    float ht = .3090f/Sin(((ang+18.0f)*.01745f));
    ang = Mod ((1.0f-texCoord1)*360.0f, 144.0f);
    float ht1 = .3090f/Sin(((ang+18.0f)*.01745f));
    ht = Max (ht, ht1);
    ht1 = ht*.5f-Min(texCoord2*2.0f, 1.0f);
    ht1 = Clamp (ht1, -ddu, ddu)/(ddu*2.0f)+.5f;
    ht = ht/2.0f - Min((1.0f-texCoord2)*2.0f, 1.0f);
    ht1 = ht1 + Clamp(ht, -ddu, ddu)/(ddu*2.0f)+.5f;

    Color ballcolor = Mix (Color(.8f,.6f,0.0f), Color (.5f,.05f,.05f), ht1);
    ballcolor = Mix (Color(0.0f,0.2f,.7f), ballcolor,
                     Clamp(Abs(texCoord2-0.5f)-0.1f, 0.0f, ddv)/ddv);

    Vector Rfldir, Rfrdir;      /* Smooth reflection/refraction directions */
    Vector uoffset, voffset;    /* Offsets for blur */
    Color ev(0.0f, 0.0f, 0.0f); /* Color of the environment reflections */
    Color cr(0.0f, 0.0f, 0.0f); /* Color of the refractions */
    float kr, kt;


    /* Construct a forward facing surface normal */
    Normal Nf = FaceForward(N, IN);

    /* Compute the reflection & refraction directions and amounts */
    Fresnel (IN, Nf, (Dot(IN, N) < 0) ? 1.0f/eta : eta, kr, kt, Rfldir, Rfrdir);
    kr *= Kr;
    kt *= Kt;

    /* Calculate the reflection color */
    if (kr > 0.001f && depth < maxDepth) {
        /* Rdir gets the perfect reflection direction */
        Vector Rdir = Normalized (Rfldir);
        if (blur > 0.0f) {
            /* Construct orthogonal components to Rdir */
            uoffset = blur * Normalized(Vector (Rdir.z - Rdir.y,
                                                Rdir.x - Rdir.z,
                                                Rdir.y - Rdir.x));
            voffset = Cross(Rdir, uoffset);
            for (float i = 0.0f;  i < samples;  i += 1.0f) {
                for (float j = 0.0f;  j < samples;  j += 1.0f) {
                    /* Add a random offset to the smooth reflection vector */
                    Vector R = Rdir +
                        ((i + Random())/samples - 0.5f) * uoffset +
                        ((j + Random())/samples - 0.5f) * voffset;

                    Vector RN = Normalized(R);
                    Color rayColor;
                    //TraceRayEps(P, RN, RAY_EPS, rayColor);
                    TraceRay(P, RN, rayColor);
                    
                    ev += rayColor;
                }
            }
            ev *= kr / (samples*samples);
        } else {
            /* No blur, just do a simple trace */
            Color rayColor;
            //TraceRayEps(P, Rdir, RAY_EPS, rayColor);
            TraceRay(P, Rdir, rayColor);
            ev = kr * rayColor;
        }
    }

    /* Calculate the refraction color */
    if (kt > 0.001f && depth < maxDepth) {
        /* Rdir gets the perfect refraction direction */
        Vector Rdir = Normalized (Rfrdir);
        if (blur > 0.0f) {
            /* Construct orthogonal components to Rdir */
            uoffset = blur * Normalized(Vector(Rfrdir.z - Rfrdir.y,
                                               Rfrdir.x - Rfrdir.z,
                                               Rfrdir.y - Rfrdir.x));
            voffset = Cross(Rfrdir, uoffset);
            for (float i = 0.0f;  i < samples;  i += 1.0f) {
                for (float j = 0.0f;  j < samples;  j += 1.0f) {
                    /* Add a random offset to the smooth reflection vector */
                    Vector R = Rdir +
                        ((i + Random())/samples - 0.5) * uoffset +
                        ((j + Random())/samples - 0.5) * voffset;

                    Color rayColor;
                    //TraceRayEps(P, R, RAY_EPS, rayColor);
                    TraceRay(P, R, rayColor);
                    
                    cr += rayColor;
                }
            }
            cr *= kt / (samples*samples);
        } else {
            /* No blur, just do a simple trace */
            Color rayColor;
            //TraceRayEps(P, Rdir, RAY_EPS, rayColor);
            TraceRay(P, Rdir, rayColor);
            cr = kt * rayColor;
        }
    }

    Color C_diffuse(0.0f, 0.0f, 0.0f);
    Color C_specular(0.0f, 0.0f, 0.0f);
    float invRoughness = 1.0f/roughness;

    BEGIN_ILLUMINANCE_LOOP(P) {
        C_diffuse += diffuseComponent(L_dir_norm, P, Nf, Cl);
        C_specular += specularComponent(L_dir_norm,
                                        P, Nf, -IN, Cl,
                                        roughness, invRoughness);
    } END_ILLUMINANCE_LOOP;

    result = (ballcolor * (Ka + Kd*C_diffuse) +
              specularcolor * (ev + Ks*C_specular) +
              transmitcolor * cr);
}
