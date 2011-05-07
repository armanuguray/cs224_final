varying vec3 normal;
varying vec4 pos;
varying vec3 vel;

uniform sampler2D overview;
uniform mat4 world_transform;

attribute vec3 velocity;

void main(void)
{
    normal = normalize((world_transform * vec4(gl_Normal, 0.0)).xyz);
    pos = world_transform * gl_Vertex;
    vel = velocity;

    gl_Position = ftransform();
}