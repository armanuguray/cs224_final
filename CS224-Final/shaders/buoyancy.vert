varying vec3 normal;
varying vec4 pos;

uniform mat4 ctm;

void main(void)
{
    normal = normalize((ctm * vec4(gl_Normal, 0.0)).xyz);
    pos = ctm * gl_Vertex;
    gl_Position = ftransform();
}