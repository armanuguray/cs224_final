
const int RADIUS = 14; // Must match WaveConstants.h
const int WIDTH  = 2 * RADIUS + 1;

uniform float weights[WIDTH];

uniform float wp_max_amplitude;
uniform float heightmap_resolution;

uniform sampler2D texture;

void main()
{
    gl_FragColor = vec4(0.0);
    float dx = 1.0 / heightmap_resolution;

    int middle = RADIUS;
    for (int i = 0; i <= RADIUS; ++i)
    {
        float weight = weights[middle + i];
        gl_FragColor += weight * texture2D(texture, gl_TexCoord[0].xy + vec2(dx * float(i), 0.0));

        weight = weights[middle - i];
        gl_FragColor += weight * texture2D(texture, gl_TexCoord[0].xy + vec2(dx * float(-i), 0.0));
    }
}
