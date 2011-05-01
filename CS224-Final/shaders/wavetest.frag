
varying vec3 n;

void main()
{
    vec3 normal = normalize(n);
    gl_FragColor = vec4(dot(normal, vec3(0.0, 1.0, 0.0)));
}
