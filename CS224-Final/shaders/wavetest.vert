
varying vec3 n;

void main()
{
    n = gl_Normal;
    gl_Position = ftransform();
}
