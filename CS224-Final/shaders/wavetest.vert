
varying vec3 n;

void main()
{
    n = gl_Normal;
    gl_Position = ftransform();
    gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;
}
