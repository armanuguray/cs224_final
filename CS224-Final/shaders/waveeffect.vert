varying vec3 normal;
varying vec4 pos;

uniform sampler2D overview;
uniform mat4 world_transform;

void main(void)
{
    normal = normalize((world_transform * vec4(gl_Normal, 0.0)).xyz);
    pos = world_transform * gl_Vertex;

    gl_Position = ftransform();
    gl_FrontColor = gl_Color;
}