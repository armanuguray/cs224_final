
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
    vec2 factors = texture2D(texture, lookup).rg;
    return wp_max_amplitude * (factors.x - factors.y);
}

vec3 gradientNormal(float dx, float dz, float wdx, float wdz, float y0, vec2 lookup)
{
    float ydx = readHeight(lookup + vec2(dx, 0.0));
    float ydz = readHeight(lookup + vec2(0.0, dz));

    return cross(vec3(wdx, ydx - y0, 0.0), vec3(0.0, ydz - y0, wdz));
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

    float y0 = readHeight(lookup);

    vec4 vert = gl_Vertex;
    n = gl_Normal;

    if (lookup.x >= 0.0 && lookup.x <= 1.0 && lookup.y >= 0.0 && lookup.y <= 1.0)
    {
        vert.y += y0;
        n = normalize(
                        gradientNormal(-delta, -delta, -dx, -dz, y0, lookup) +
                       -gradientNormal( delta, -delta,  dx, -dz, y0, lookup) +
                       -gradientNormal(-delta,  delta, -dx,  dz, y0, lookup) +
                        gradientNormal( delta,  delta,  dx,  dz, y0, lookup)
                     );
    }

    gl_Position = gl_ModelViewProjectionMatrix * vert;
    view = normalize(vert.xyz - (gl_ModelViewMatrixInverse * vec4(0, 0, 0, 1)).xyz);
}
