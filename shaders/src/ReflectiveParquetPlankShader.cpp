#include "RTfactShader.hpp"

/*
Parquet plank shader : another shader for wood
based on parquet_plank.sl :

Author:
written by Larry Gritz, the George Washington University
10 Feb 1995 - written by Larry Gritz, based on "plank" shader.
last modified 10 Feb 1995 by Larry Gritz

Original description:
Makes texture of wooden planks in s-t space.  This wood looks rather
like oak plank parquet floor tiles.  The actual wood & plank pattern
is based on my planks shader.  This shader works best if "s" and "t"
units are both the same size in world space.
*/

// following parameters work like plastic shader
PARAMETER float Ka;
PARAMETER float Kd;
PARAMETER float Ks;
PARAMETER float Kr;       /* reflective coefficient */
PARAMETER float eta;      /* index of refraction */
PARAMETER float roughness;
PARAMETER Color specularcolor;

PARAMETER float ringscale;  // scaling for the ring spacing
PARAMETER float grainscale; // scaling for the fine grain
PARAMETER float ts_x; // overall scaling factor for the texture X
PARAMETER float ts_y; // overall scaling factor for the texture Y
PARAMETER float plankspertile; // number of planks in each parquet tile

// surface colors for the wood itself
PARAMETER Color lightwood;
PARAMETER Color darkwood;

// the color of the "grooves" between the planks
PARAMETER Color groovecolor;

// width of each plank (in terms of s/t)
PARAMETER float plankwidth;

// width of grooves between planks (in terms of s/t)
PARAMETER float groovewidth;

// controls how much color varies from plank to plank
PARAMETER float plankvary;

// relative graininess (0 = no fine grain)
PARAMETER float grainy;

// relative wavyness of the ring pattern
PARAMETER float wavy;

PARAMETER float maxDepth;
PARAMETER float reflectance;

BEGIN_INIT_PARAM
INIT_PARAM(Ka, 1.0f)
INIT_PARAM(Kd, 0.75f)
INIT_PARAM(Ks, 0.15f)
INIT_PARAM_DESCR(Kr, 1.0f, "reflective coefficient")
INIT_PARAM_DESCR(eta, 1.5f, "index of refraction")
INIT_PARAM(roughness, 0.025f)
INIT_PARAM(specularcolor, Color(1.0f, 1.0f, 1.0f))
INIT_PARAM_DESCR(ringscale, 15.0f, "scaling for the ring spacing")
INIT_PARAM_DESCR(grainscale, 60.0f, "scaling for the fine grain")
INIT_PARAM_DESCR(ts_x, 1.0f, "overall scaling factor for the texture X")
INIT_PARAM_DESCR(ts_y, 1.0f, "overall scaling factor for the texture Y")
INIT_PARAM_DESCR(plankspertile, 4.0f, "number of planks in each parquet tile")
INIT_PARAM(lightwood, Color(0.57f, 0.292f, 0.125f))
INIT_PARAM(darkwood, Color(0.275f, 0.15f, 0.06f))
INIT_PARAM_DESCR(groovecolor, Color(.05f, .04f, .015f),
                 "the color of the \"grooves\" between the planks")
INIT_PARAM_DESCR(plankwidth, 0.05f, "width of each plank (in terms of s/t)")
INIT_PARAM_DESCR(groovewidth, 0.001f,
                 "width of grooves between planks (in terms of s/t)")
INIT_PARAM_DESCR(plankvary, 0.8f,
                 "controls how much color varies from plank to plank")
INIT_PARAM_DESCR(grainy, 1.0f, "relative graininess (0 = no fine grain)")
INIT_PARAM_DESCR(wavy, 0.08f, "relative wavyness of the ring pattern")
INIT_PARAM(maxDepth, 2.f)
INIT_PARAM(reflectance, .5f)
END_INIT_PARAM

SURFACE_SHADER
{

        const Vector &I = dir; /* normalized incident vector */
        Point P = origin + hitDistance * dir;
        const Normal &N = normal;

        float PGWIDTH = plankwidth+groovewidth;
        float planklength = PGWIDTH * plankspertile - groovewidth;
        float PGHEIGHT = planklength+groovewidth;
        float GWF = groovewidth*0.5f/PGWIDTH;
        float GHF = groovewidth*0.5f/PGHEIGHT;

        /* Determine how wide in s-t space one pixel projects to */
        float swidth = 0.05f*ts_x;
        float twidth = 0.05f*ts_y;
        float fwidth = Max(swidth,twidth);

        Normal Nf = FaceForward(N, I);

        float ss = (ts_x * texCoord1) / PGWIDTH;
        float whichrow = Floor(ss);
        float tt = (ts_y * texCoord2) / PGHEIGHT;
        float whichplank = Floor(tt);
        if (Mod (whichrow/plankspertile + whichplank, 2.f) >= 1.f)
        {
            ss = ts_x * texCoord2 / PGWIDTH;
            whichrow = Floor (ss);
            tt = ts_y * texCoord1 / PGHEIGHT;
            whichplank = Floor(tt);
            float tmp = swidth;  swidth = twidth;  twidth = tmp;
        }
        ss -= whichrow;
        tt -= whichplank;
        whichplank += 20.f*(whichrow+10.f);

        /*
         * Figure out where the grooves are.  The value groovy is 0 during
         * are grooves, 1 where the wood grain is visible.
         */
        /* This would be the non-antialiased version: */
        float w = Step (GWF,ss) - Step(1-GWF,ss);
        float h = Step (GHF,tt) - Step(1-GHF,tt);
        float groovy = w*h;

        /*
         * Add the ring patterns
         */
        float r;
        float fade = SmoothStep (1.f/ringscale, 8.f/ringscale, fwidth);
        if (fade < 0.999f)
        {
            float ttt = tt/4.f+whichplank/28.38f + wavy * Noise (8.f*ss, tt/4.f);
            r = ringscale * Noise (ss-whichplank, ttt);
            r -= Floor (r);
            r = 0.3f+0.7f*SmoothStep(0.2f, 0.55f, r)*(1-SmoothStep(0.75f, 0.8f, r));
            r = (1.f-fade)*r + 0.65f*fade;

            /*
             * Multiply the ring pattern by the fine grain
             */
            fade = SmoothStep (2.f/grainscale, 8.f/grainscale, fwidth);
            if (fade < 0.999f) {
                float r2 = 1.3f - Noise (ss*grainscale, (tt*grainscale/4.f));
                r2 = grainy * r2*r2 + (1.f-grainy);
                r *= (1.f-fade)*r2 + (0.75f*fade);
            }
            else r *= 0.75f;
        }
        else r = 0.4875f;

        /* Mix the light and dark wood according to the grain pattern */
        Color woodcolor = Mix (lightwood, darkwood, r);

        /* Add plank-to-plank variation in overall color */
        woodcolor *= (1.f-plankvary/2.f + plankvary * Noise (whichplank+0.5f));

        Color Ct = Mix (groovecolor, woodcolor, groovy);

        /* Use the plastic illumination model */
        // Ci = Os * ( Ct * (Ka*ambient() + Kd*diffuse(Nf)) +
        //	       specularcolor * Ks*specular(Nf,-normalize(I),roughness));

        Color C_diffuse(0.0f, 0.0f, 0.0f);
        Color C_specular(0.0f, 0.0f, 0.0f);
        float invRoughness = 1.0f/roughness;

        

        BEGIN_ILLUMINANCE_LOOP(P) {
            C_specular += specularComponent(L_dir_norm,
                                            P, Nf, -I, Cl,
                                            roughness, invRoughness);
            C_diffuse += diffuseComponent(L_dir_norm, P, Nf, Cl);
        } END_ILLUMINANCE_LOOP;

        Color Ci = Ct * (Ka + Kd  * C_diffuse) + specularcolor * Ks * C_specular;

#if 0       
		/* reflection */
        Color rayColor;
        float kr, kt;
        Fresnel(I, Nf, (Dot(I, N) < 0) ? 1.0f/eta : eta, kr, kt);
        kr *= Kr;

        Vector R = Reflect( I, Nf );
        
        if (depth < maxDepth)
        {
            //TraceRay(P + RAY_EPS * R, R, rayColor);
            TraceRay(P, R, rayColor);
            Ci += kr * rayColor;
        }
        result = Ci;
#else
	Color Crefl;
	Point reflDir = Reflect(dir, Nf);
	if (depth < maxDepth) {
        //TraceRayEps(P, reflDir, RAY_EPS, result);
        TraceRay(P, reflDir, Crefl);
        result = (Ci * (Ka + Kd * C_diffuse)) * (1.f-reflectance) + Crefl * reflectance;
    } else {
        result = Ci * (Ka + Kd  * C_diffuse);
    }
#endif

}
