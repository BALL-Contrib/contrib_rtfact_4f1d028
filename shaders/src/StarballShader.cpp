/************************************************************************
 * starball.sl - make a striped ball with a star pattern
 *
 * Reference:
 *   _Advanced RenderMan: Creating CGI for Motion Picture_, 
 *   by Anthony A. Apodaca and Larry Gritz, Morgan Kaufmann, 1999.
 ************************************************************************/

#include "RTfactShader.hpp"
#include "Material.hpp"

PARAMETER float Ka;
PARAMETER float Kd;
PARAMETER float Ks;
PARAMETER float roughness;

BEGIN_INIT_PARAM
INIT_PARAM(Ka, 1.0f)
INIT_PARAM(Kd, 0.5f)
INIT_PARAM(Ks, 0.5f)
INIT_PARAM(roughness, 0.1f)
END_INIT_PARAM

SURFACE_SHADER
{
	const Point P = origin + hitDistance * dir;
    const Vector & IN = dir;  /* normalized incident vector */
    const Normal & N = normal;  /* surface normal */

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

    Color Ct = Mix (Color(.8f,.6f,0.0f), Color (.5f,.05f,.05f), ht1);
    Ct = Mix (Color(0.0f,0.2f,.7f), Ct, 
              Clamp(Abs(texCoord2-0.5f)-0.1f, 0.0f, ddv)/ddv);

    Normal Nf = FaceForward (N, IN);
    result = MaterialPlastic(CTX_ARG, P, IN, Nf, Ct, Ka, Kd, Ks, roughness);
}
