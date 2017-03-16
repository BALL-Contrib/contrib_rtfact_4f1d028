#include "RTfactShader.hpp"

//parameters
PARAMETER Color color;
PARAMETER float Ka;
PARAMETER float Kd;
PARAMETER float Ks;
PARAMETER float roughness;
PARAMETER float frequency;

BEGIN_INIT_PARAM
INIT_PARAM(color, Color(1.0f, 1.0f, 1.0f))
INIT_PARAM(Ka, 0.2f)
INIT_PARAM(Kd, 0.4f)
INIT_PARAM(Ks, 0.1f)
INIT_PARAM(roughness, 0.1f)
INIT_PARAM(frequency, 1.f)
END_INIT_PARAM


SURFACE_SHADER
{
    const Vector &IN = dir; /* normalized incident vector */
	const Point P = origin + hitDistance * dir;
    const Normal &N = normal;

	float sum = 0;
	float i, freq = frequency; // freq = 1.0f; /* Try other values for example, 7.0 */

	for (i = 0; i < 6; i = i + 1) {
		sum = sum + Abs(.5f - Noise(4.0f * freq* IN * hitDistance)) / freq;
		freq*= 2.0f;
	}

    Normal Nf = FaceForward(N, IN);

    Color C_diffuse(0.0f, 0.0f, 0.0f);
    Color C_specular(0.0f, 0.0f, 0.0f);
    float invRoughness = 1.0f/roughness;

    BEGIN_ILLUMINANCE_LOOP(P) {
        C_diffuse += diffuseComponent(L_dir_norm, P, Nf, Cl);
        C_specular += specularComponent(L_dir_norm,
                                        P, Nf, -IN, Cl,
                                        roughness, invRoughness);
    } END_ILLUMINANCE_LOOP;

    result = color * sum * (Ka + Kd * C_diffuse + Ks * C_specular);
}
