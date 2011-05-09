
uniform vec2 wp_spawn_point;
uniform float wp_time;
uniform vec2 wp_direction;

uniform vec2 heightmap_center;
uniform vec2 heightmap_size;
uniform float time;
uniform float velocity;

void main()
{
    vec2 pos = wp_spawn_point + wp_direction * velocity * (time - wp_time);

    vec4 vert = gl_Vertex;
    vert.xy += (pos - heightmap_center + .5 * heightmap_size) / heightmap_size;
    vert.y = 1.0 - vert.y;

    gl_Position = gl_ProjectionMatrix * vert;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
