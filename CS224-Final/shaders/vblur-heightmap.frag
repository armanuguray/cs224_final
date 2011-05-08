
const int RADIUS = 2; // Must match WaveConstants.h
const int WIDTH  = RADIUS + 1;

uniform vec3 weights[WIDTH];

uniform float wp_max_amplitude;
uniform float heightmap_resolution;

uniform sampler2D texture;

void main()
{
    gl_FragColor = vec4(0.0);
    float dz = 1.0 / heightmap_resolution;
    vec3 c = vec3(0.0);

    for (int i = 0; i <= RADIUS; ++i)
    {
        vec3 w = weights[i];
        float r = float(i);
        c += w * (2.0 * texture2D(texture, gl_TexCoord[0].xy + vec2(0.0, dz *  r)).rgb - vec3(1.0));
        c += w * (2.0 * texture2D(texture, gl_TexCoord[0].xy + vec2(0.0, dz * -r)).rgb - vec3(1.0));
    }

    gl_FragColor = vec4(vec3(.5) + .5 * c, 0.0);
}
