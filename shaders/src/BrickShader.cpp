#include "RTfactShader.hpp"

//parameters
PARAMETER Color ambientColor;
PARAMETER Color diffuseColor;
PARAMETER Color specularColor;
PARAMETER float kg;
PARAMETER Color brickColor;
PARAMETER Color mortarColor;
PARAMETER float brickWidth;
PARAMETER float brickHeight;
PARAMETER float mortarThickness;
PARAMETER float s_scale;
PARAMETER float t_scale;

BEGIN_INIT_PARAM
INIT_PARAM(ambientColor, Color(0.1f, 0.1f, 0.1f))
INIT_PARAM(diffuseColor, Color(1.0f, 1.0f, 1.0f))
INIT_PARAM(specularColor, Color(0.6f, 0.6f, 0.6f))
INIT_PARAM(kg, 0.9f)
INIT_PARAM(brickColor, Color(1.0f, 0.15f, 0.14f))
INIT_PARAM(mortarColor, Color(0.4f, 0.4f, 0.4f))
INIT_PARAM(brickWidth, 0.25f)
INIT_PARAM(brickHeight, 0.08f)
INIT_PARAM(mortarThickness, 0.01f)
INIT_PARAM(s_scale, 1.0f)
INIT_PARAM(t_scale, 1.0f)
END_INIT_PARAM

SURFACE_SHADER
{
	const Point P = origin + hitDistance * dir;
    const Vector & IN = dir;  /* normalized incident vector */
    const Normal & N = normal;  /* surface normal */

	//precomputed values
	float bmwidth = brickWidth + mortarThickness;
	float bmheight = brickHeight + mortarThickness;
	float mwf = mortarThickness * 0.5f / bmwidth;
	float mhf = mortarThickness * 0.5f / bmheight;

	//shader

    // Texture coordinates
    float scoord = texCoord1 * s_scale;
    float tcoord = texCoord2 * t_scale;

	float ss = scoord / bmwidth;
	float tt = tcoord / bmheight;

	// shift alternate rows
	if (Mod(tt * 0.5f, 1.0f) > 0.5f) ss += 0.5f;

	float sbrick = Floor(ss); /* which brick? */
	float tbrick = Floor(tt); /* which brick? */
    //ss += 0.1*Noise(tbrick+0.5);

	ss -= sbrick;
	tt -= tbrick;

    float w = Step(mwf, ss) - Step(1.0f-mwf,ss);
    float h = Step(mhf, tt) - Step(1.0f-mhf,tt);

    Color Ct = Mix(mortarColor, brickColor, w*h);

    // compute bump-mapping function for mortar grooves
    float sbump = SmoothStep(0.0, mwf, ss) - SmoothStep(1.0-mwf,1.0,ss);
    float tbump = SmoothStep(0.0, mhf, tt) - SmoothStep(1.0-mhf,1.0,tt);

    // compute shading normal
	Point U, V;
	GetFrame(N, U, V);
	Normal Nf = FaceForward(Normalized(N + 0.8f*(1.f - sbump)*U + 0.8f*(1.f - tbump)*V), IN);


    Color C_diffuse(0.0f, 0.0f, 0.0f);
    Color C_phong(0.0f, 0.0f, 0.0f);

    const Vector R = Reflect(IN, N);

    BEGIN_ILLUMINANCE_LOOP(P) {
        C_diffuse += diffuseComponent(L_dir_norm, P, Nf, Cl);
        C_phong += phongComponent(L_dir_norm,
                                  P, Nf, R, Cl,
                                  kg);
    } END_ILLUMINANCE_LOOP;

    result = Ct*(ambientColor + diffuseColor * C_diffuse + specularColor*C_phong);
}

