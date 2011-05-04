
float PI = 3.14159265358979323846264;

uniform float wp_max_amplitude;

// Heightmap corner world positions
uniform vec3 tl;
uniform vec3 tr;
uniform vec3 bl;
uniform vec3 br;

// Wave particle parameters
uniform vec2  wp_pos;       // Position of the wave particle, in world space
uniform float wp_amplitude; // Amplitude of the particle, above or below the height map, in world coordinates
uniform float wp_radius;    // The area of effect of the particle, in world coordinates

void main()
{
    vec3 world = mix(mix(tl, tr, gl_TexCoord[0].x),
                     mix(bl, br, gl_TexCoord[0].x),
                     gl_TexCoord[0].y);

    float dist = length(world.xz - wp_pos.xy);

    float amp = 0;
    if (dist < wp_radius)
        amp = ((.5 * wp_amplitude) * cos(PI * dist / wp_radius));

    // Awesometastic hacks for additive blending!
    gl_FragColor = vec4(0.0);
    if (amp > 0.0)
        gl_FragColor.r = amp / wp_max_amplitude;
    else
        gl_FragColor.g = -amp / wp_max_amplitude;
}
