
const int RADIUS = 2; // Must match WaveConstants.h
const int WIDTH  = RADIUS + 1;

uniform vec3 heightWeights[WIDTH];
uniform vec2 velocityWeights[WIDTH];

uniform float wp_max_amplitude;
uniform float heightmap_resolution;

uniform sampler2D texture;
uniform sampler2D velocity;

void main()
{
    gl_FragData[0] = vec4(0.0);
    float dz = 1.0 / heightmap_resolution;
    vec3 h = vec3(0.0);
    vec2 v = vec2(0.0);

    for (int i = 0; i <= RADIUS; ++i)
    {
        vec3 hw = heightWeights[i];
        vec2 vw = velocityWeights[i];

        float r = float(i);
        h += hw * (2.0 * texture2D(texture, gl_TexCoord[0].xy + vec2(0.0, dz *  r)).rgb - vec3(1.0));
        h += hw * (2.0 * texture2D(texture, gl_TexCoord[0].xy + vec2(0.0, dz * -r)).rgb - vec3(1.0));

        v += vw * (2.0 * texture2D(velocity, gl_TexCoord[0].xy + vec2(0.0, dz *  r)).rg - vec2(1.0));
        v += vw * (2.0 * texture2D(velocity, gl_TexCoord[0].xy + vec2(0.0, dz * -r)).rg - vec2(1.0));
    }

    gl_FragData[0] = vec4(vec3(.5) + .5 * h, 0.0);
    gl_FragData[1].xy = vec2(.5) + .5 * v;
}
