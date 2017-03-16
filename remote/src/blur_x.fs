void main()
{
    gl_FragColor.r=blur(
        vec2(1.0,0.0),
        vec3(gl_TexCoord[0].xy, texture2DRect(uDistanceTexture,gl_TexCoord[0].xy)));
}
