// outputs
varying vec3 F_lift;
varying vec3 F_drag;

void main(void)
{
    gl_FragColor = vec4(F_lift + F_drag, 1.0);
}