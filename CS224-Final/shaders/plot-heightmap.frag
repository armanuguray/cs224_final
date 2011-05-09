
uniform float wp_amplitude;
uniform float max_amplitude;

void main()
{
    gl_FragColor = vec4(wp_amplitude / max_amplitude, -wp_amplitude / max_amplitude, 0.0, 1.0);
}
