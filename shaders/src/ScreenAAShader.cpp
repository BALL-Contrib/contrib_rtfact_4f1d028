#include "RTfactShader.hpp"

/*
 * screen_aa.sl -- RenderMan compatible shader for a metalic screen.
 *
 * DESCRIPTION:
 * Makes a surface that looks like a metal screen. Strips of metal run
 * parallel to lines of s and t.  You can adjust the Ka, Kd, Ks, etc.
 * to change the material appearance.  This texture antialiases pretty
 * well, even with only one sample per pixel.
 *
 * PARAMETERS:
 * Ka, Kd, Ks, roughness, specularcolor - work like the plastic shader
 *   frequency - how many cycles of screen in st space
 *   density - how much of each cycle is opaque?
 *
 * AUTHOR: written by Larry Gritz
 */ 



PARAMETER Color Cs;
PARAMETER Color specularcolor;
PARAMETER float Ka; // ambient
PARAMETER float Kd; // diffuse
PARAMETER float Ks; // specular
PARAMETER float roughness;
PARAMETER float density;
PARAMETER float frequency;

BEGIN_INIT_PARAM
INIT_PARAM(Cs, Color(0.0f,0.0f,1.0f))
INIT_PARAM(specularcolor, Color(1.0f,0.0f,0.0f))
INIT_PARAM_DESCR(Ka, 0.2f, "ambient")
INIT_PARAM_DESCR(Kd, 0.1f, "diffuse")
INIT_PARAM_DESCR(Ks, 0.8f, "specular")
INIT_PARAM(roughness, 0.1f)
INIT_PARAM(density, 0.20f)
INIT_PARAM(frequency, 20.0f)
END_INIT_PARAM

SURFACE_SHADER
{
    const Point & IN = dir; /* normalized incident vector */
    float d;      /* Density at the sample point */
    float ss, tt; /* s,t, parameters in phase */
    
    /* Compute a forward facing normal */
    Point Nf = FaceForward(normal, IN);

    /* Figure out where in the pattern we are */
    ss = Mod (frequency * texCoord1, 1);
    tt = Mod (frequency * texCoord2, 1);

    float GWF = density*0.5;


    /* This would be the non-antialiased version: */
    float w = Step (GWF,ss) - Step(1-GWF,ss);
    float h = Step (GWF,tt) - Step(1-GWF,tt);
    d = 1 - w*h;

    Point P = origin + hitDistance * dir;

    if (d > 0)
    {
        Color C_diffuse(0.0f, 0.0f, 0.0f);
        Color C_specular(0.0f, 0.0f, 0.0f);
        float invRoughness = 1.0f/roughness;

        BEGIN_ILLUMINANCE_LOOP(P) {
            C_specular += specularComponent(L_dir_norm,
                                            P, Nf, -IN, Cl,
                                            roughness, invRoughness);
            C_diffuse += diffuseComponent(L_dir_norm, P, Nf, Cl);
        } END_ILLUMINANCE_LOOP;

        result = d*(Cs * (Ka + Kd  * C_diffuse) + specularcolor * Ks * C_specular);

        return;
    }

    //TraceRay(P + 10.0f * EPS * IN, IN, result);
	//TraceRay(P, IN, results); //causes problems...
	Color Crefl;
	TraceRay(P, IN, Crefl);
	result = Crefl;
}
