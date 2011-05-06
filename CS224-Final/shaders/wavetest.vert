
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

float readHeight(vec2 lookup)
{
    return wp_max_amplitude * (2.0 * texture2D(texture, lookup).y - 1.0);
}

vec3 gradientNormal(float hdx, float hdz, float wdx, float wdz, vec3 v0, vec3 world, vec2 lookup)
{
    vec3 vdx = world + vec3(wdx, 0.0, 0.0) + wp_max_amplitude * (2.0 * texture2D(texture, lookup + vec2(hdx, 0.0)).xyz - vec3(1.0));
    vec3 vdz = world + vec3(0.0, 0.0, wdz) + wp_max_amplitude * (2.0 * texture2D(texture, lookup + vec2(0.0, hdz)).xyz - vec3(1.0));

    return cross(vdx - v0, vdz - v0);
}

void main()
{
    gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;

    vec3 world = mix(mix(tl, tr, gl_TexCoord[0].x),
                     mix(bl, br, gl_TexCoord[0].x),
                     1.0 - gl_TexCoord[0].y);

    vec2 lookup = vec2((world.x - htl.x) / (htr.x - htl.x),
                       (world.z - htr.z) / (hbr.z - htr.z));

    float delta = 1.0 / (heightmap_resolution);
    float dx = (htr.x - htl.x) * delta;
    float dz = (htr.z - hbr.z) * delta;

    vec3 dvert = wp_max_amplitude * (2.0 * texture2D(texture, lookup).xyz - vec3(1.0));

    vec4 vert = gl_Vertex;
    n = gl_Normal;

    if (lookup.x >= 0.0 && lookup.x <= 1.0 && lookup.y >= 0.0 && lookup.y <= 1.0)
    {
        vert.xyz += dvert;
        n = normalize(
                        gradientNormal(-delta, -delta, -dx, -dz, vert.xyz, world, lookup) +
                       -gradientNormal( delta, -delta,  dx, -dz, vert.xyz, world, lookup) +
                       -gradientNormal(-delta,  delta, -dx,  dz, vert.xyz, world, lookup) +
                        gradientNormal( delta,  delta,  dx,  dz, vert.xyz, world, lookup)
                     );
    }

    gl_Position = gl_ModelViewProjectionMatrix * vert;
    view = normalize(vert.xyz - (gl_ModelViewMatrixInverse * vec4(0, 0, 0, 1)).xyz);
}
