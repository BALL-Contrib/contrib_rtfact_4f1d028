#include "RTfactShader.hpp"
#include <cstdio>

PARAMETER float ringscale;
PARAMETER Color lightWood;
PARAMETER Color darkWood;
PARAMETER float Ka;
PARAMETER float Kd;
PARAMETER float Ks;
PARAMETER float roughness;

BEGIN_INIT_PARAM
INIT_PARAM(ringscale, 100.0f)
INIT_PARAM(lightWood, Color(0.3f, 0.12f, 0.03f))
INIT_PARAM(darkWood, Color(0.05f, 0.01f, 0.005f))
INIT_PARAM(Ka, 0.2f)
INIT_PARAM(Kd, 0.4f)
INIT_PARAM(Ks, 0.6f)
INIT_PARAM(roughness, 0.1f)
END_INIT_PARAM

SURFACE_SHADER
{
    const Point P = origin + hitDistance * dir;
    const Vector & IN = dir;  /* normalized incident vector */
    const Normal & N = normal;  /* surface normal */

  /* 
   * Compute the forward-facing normal NN and the vector
   * toward the ray orgigin V, both normalized.
   * These vectors are used by "specular" and "diffuse". */
    Normal NN = FaceForward(N, IN);
    Vector V = -IN;

    Point PP = P; // transform("shader", P);
    PP += Noise(PP);

    /* compute radial distance r from PP to axis of "tree" */
    float r = Sqrt (PP.y * PP.y + PP.z * PP.z);

    /* map radial distance r nto ring position [0, 1] */
    r *= ringscale;
    r += Abs (Noise(r));
    r -= Floor (r);         /* == mod (r, 1) */

    /* use ring poisition r to select wood color */
    r = SmoothStep (0.f, 0.8f, r) - SmoothStep (0.83f, 1.0f, r);
    Color Ci = Mix(lightWood, darkWood, r);

    /* shade using r to vary shininess */
    // Oi = Os;
    //Ci = Oi * Ci * (Ka * ambient() + Kd * diffuse(NN))
    //  + (0.3 * r + 0.7) * Ks * specular (NN, V, roughness);

    Color C_diffuse(0.0f, 0.0f, 0.0f);
    Color C_specular(0.0f, 0.0f, 0.0f);
    float invRoughness = 1.0f/roughness;

    BEGIN_ILLUMINANCE_LOOP(P) {
        C_diffuse += diffuseComponent(L_dir_norm, P, NN, Cl);
        C_specular += specularComponent(L_dir_norm,
                                        P, NN, V, Cl,
                                        roughness, invRoughness);
    } END_ILLUMINANCE_LOOP;

    result = Ci * (Ka + Kd  * C_diffuse) + (0.3f * r + 0.7f) * Ks * C_specular;
}
