
varying vec3 n;
varying vec3 view;

uniform float wp_max_amplitude;
uniform float heightmap_resolution;

uniform sampler2D texture;

// Corners of the projected grid
uniform vec3 tl;
uniform vec3 tr;
uniform vec3 bl;
uniform vec3 br;

// Corners of the heightmap
uniform vec3 htl;
uniform vec3 htr;
uniform vec3 hbl;
uniform vec3 hbr;

varying vec3 test;

void main()
{
    gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;

    vec3 world = mix(mix(tl, tr, gl_TexCoord[0].x),
                     mix(bl, br, gl_TexCoord[0].x),
                     1.0 - gl_TexCoord[0].y);

    vec2 lookup = vec2((world.x - htl.x) / (htr.x - htl.x),
                       (world.z - htr.z) / (hbr.z - htr.z));

    float delta = 1.0 / (heightmap_resolution * .0625);
    float dx = (htr.x - htl.x) * delta;
    float dz = (htr.z - hbr.z) * delta;
    float y0 = wp_max_amplitude * texture2D(texture, lookup).r;
    y0 -= wp_max_amplitude * texture2D(texture, lookup).g;
    float y1 = wp_max_amplitude * texture2D(texture, lookup + vec2(delta, 0.0)).r;
    y1 -= wp_max_amplitude * texture2D(texture, lookup + vec2(delta, 0.0)).g;
    float y2 = wp_max_amplitude * texture2D(texture, lookup + vec2(0.0, delta)).r;
    y2 -= wp_max_amplitude * texture2D(texture, lookup + vec2(0.0, delta)).g;

    test = vec3(y0, y1, y2);

    vec4 vert = gl_Vertex;

    if (lookup.x >= 0.0 && lookup.x <= 1.0 && lookup.y >= 0.0 && lookup.y <= 1.0)
        vert.y += y0;

    n = cross(normalize(vec3(dx, y1 - y0, 0.0)), normalize(vec3(0.0, y2 - y0, dz)));
    view = normalize(gl_Vertex.xyz - (gl_ModelViewMatrixInverse * vec4(0, 0, 0, 1)).xyz);
    gl_Position = gl_ModelViewProjectionMatrix * vert;
}
