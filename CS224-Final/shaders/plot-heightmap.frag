
uniform float wp_amplitude;
uniform float max_amplitude;

uniform vec2 wp_direction;

void main()
{
    float signflag = 0.0;
    if (wp_direction.x < 0.0)
        signflag += 1.0;
    if (wp_direction.y < 0.0)
        signflag += 2.0;

    gl_FragData[0] = vec4(wp_amplitude / max_amplitude, -wp_amplitude / max_amplitude, signflag * .25, 1.0);
    gl_FragData[1].rg = abs(wp_direction);
}
