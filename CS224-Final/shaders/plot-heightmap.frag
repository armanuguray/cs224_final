
uniform float amplitude;
uniform float wp_max_amplitude;

void main()
{
    gl_FragColor = vec4(amplitude, -amplitude, 0.0, 1.0);
}
