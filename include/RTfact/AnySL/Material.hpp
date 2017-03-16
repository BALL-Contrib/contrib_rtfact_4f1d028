/************************************************************************
 * material.h - Functions which compute the light response of materials.
 *
 * Author: Larry Gritz (gritzl@acm.org)
 *
 * Reference:
 *   _Advanced RenderMan: Creating CGI for Motion Picture_, 
 *   by Anthony A. Apodaca and Larry Gritz, Morgan Kaufmann, 1999.
 *
 * $Revision: 1.1 $    $Date: 2000/08/28 01:30:35 $
 *
 ************************************************************************/

#ifndef MATERIAL_HPP_
#define MATERIAL_HPP_

#include "RTfactShader.hpp"

#define SURFACE_CTX void *context
#define CTX_ARG context

/* Compute the color of the surface using a simple plastic-like BRDF.
 * Typical values are Ka=1, Kd=0.8, Ks=0.5, roughness=0.1.
 */
inline Color MaterialPlastic(SURFACE_CTX,
                             const Point &P,    // surface point
                             const Vector &IN,  // normalized incident vector
                             const Normal &Nf,  // faceforwarded normal
                             const Color &basecolor,
                             float Ka,
                             float Kd,
                             float Ks,
                             float roughness)
{
    Color C_diffuse(0.0f, 0.0f, 0.0f);
    Color C_specular(0.0f, 0.0f, 0.0f);
    float invRoughness = 1.0f/roughness;

    BEGIN_ILLUMINANCE_LOOP(P) {
        C_diffuse += diffuseComponent(L_dir_norm, P, Nf, Cl);
        C_specular += specularComponent(L_dir_norm,
                                        P, Nf, -IN, Cl,
                                        roughness, invRoughness);
    } END_ILLUMINANCE_LOOP;

    return basecolor * (Ka + Kd * C_diffuse) + Ks * C_specular;
}


#endif
