varying vec3 normal;
varying vec4 pos;

uniform mat4 ctm;

void main(void)
{
    normal = normalize(gl_NormalMatrix * gl_Normal);
    pos = ctm * gl_Vertex;
    gl_Position = ftransform();
}