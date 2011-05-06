
const int RADIUS = 28; // Must match WaveConstants.h
const int WIDTH  = 2 * RADIUS + 1;

uniform float xweights[WIDTH];
uniform float yweights[WIDTH];
uniform float zweights[WIDTH];

uniform float wp_max_amplitude;
uniform float heightmap_resolution;

uniform sampler2D texture;

void main()
{
    gl_FragColor = vec4(0.0);
    float dx = 1.0 / heightmap_resolution;
    vec3 c = vec3(0.0);

    int middle = RADIUS;
    for (int i = 0; i <= RADIUS; ++i)
    {
        float r = float(i);
        vec2 tmp = texture2D(texture, gl_TexCoord[0].xy + vec2(dx * r, 0.0)).rg;
        float amp = tmp.r - tmp.g;
        c.x += xweights[middle + i] * amp;
        c.y += yweights[middle + i] * amp;
        c.z += zweights[middle + i] * amp;

        r *= -1.0;
        tmp = texture2D(texture, gl_TexCoord[0].xy + vec2(dx * r, 0.0)).rg;
        amp = tmp.r - tmp.g;
        c.x += xweights[middle - i] * amp;
        c.y += yweights[middle - i] * amp;
        c.z += zweights[middle - i] * amp;
    }

    gl_FragColor = vec4(vec3(.5) + .5 * c, 0.0);
}
