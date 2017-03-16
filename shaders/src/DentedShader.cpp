/*
 * dented.sl -- displacement shader for dents
 *
 * DESCRIPTION:
 *   Makes a surface appear worn and dented.
 * 
 * PARAMETERS:
 *   Km      the amplitude of the dents
 *   power     controls the fractal dimension of the dents (1 looks
 *             like previously crumpled paper or cloth, 3 looks
 *             like worn metal).
 *   frequency   the frequency of the dents
 *
 *
 * AUTHOR: written by Larry Gritz, based on the "dented" shader in
 *         RenderMan Companion, but with more control parameters.
 *
 * HISTORY:
 *      Dec 1992 -- written by lg for "Timbre Trees Examples" (jingle)
 *
 * last modified  12 Jan 1994 by Larry Gritz
 *
 * The RenderMan (R) Interface Procedures and RIB Protocol are:
 *     Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan (R) is a registered trademark of Pixar.
 */

#include "RTfactShader.hpp"

//parameters

PARAMETER float Km;
PARAMETER float power;
PARAMETER float frequency;
PARAMETER float maxoctaves;
PARAMETER Color ambientColor;
PARAMETER Color diffuseColor;
PARAMETER Color specularColor;
PARAMETER float kg;

BEGIN_INIT_PARAM
INIT_PARAM(Km, 1.0f)
INIT_PARAM(power, 2.0f)
INIT_PARAM(frequency, 1.0f)
INIT_PARAM(maxoctaves, 6.0f)
INIT_PARAM(ambientColor, Color(0.1f, 0.1f, 0.1f))
INIT_PARAM(diffuseColor, Color(0.3f, 0.2f, 0.1f))
INIT_PARAM(specularColor, Color(0.6f, 0.4f, 0.2f))
INIT_PARAM(kg, 0.9f)
END_INIT_PARAM

SURFACE_SHADER
{
	const Point P = origin + hitDistance * dir;
    const Vector & IN = dir;  /* normalized incident vector */
    const Normal & N = normal;  /* surface normal */

    const Point PP = P*20.0f; // transform ("shader", P);

    float magnitude = 0.0f;

    float size = frequency;
    for (float i = 0.0f;  i < maxoctaves;  i += 1.0f)
    {
        magnitude += Abs (.5f - Noise (PP*size)) / size;
        size *= 2.0f;
    }

    float bump = (Km * Pow (magnitude, power));

    // compute shading normal
	Point U, V;
	GetFrame(N, U, V);
	Normal Nf = FaceForward(Normalized(N + 0.8f*bump*U + 0.8f*bump*V), IN);
    //const Normal Nf = FaceForward(N, IN);

    Color C_diffuse(0.0f, 0.0f, 0.0f);
    Color C_phong(0.0f, 0.0f, 0.0f);

    const Vector R = Reflect(IN, N);

    BEGIN_ILLUMINANCE_LOOP(P) {
        C_diffuse += diffuseComponent(L_dir_norm, P, Nf, Cl);
        C_phong += phongComponent(L_dir_norm,
                                  P, Nf, R, Cl,
                                  kg);
    } END_ILLUMINANCE_LOOP;
    
    result = bump*(ambientColor + diffuseColor * C_diffuse + specularColor*C_phong);
}
