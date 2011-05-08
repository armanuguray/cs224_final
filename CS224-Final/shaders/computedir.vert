uniform sampler2D silhouette;
uniform sampler2D effects;

void main(void)
{
    gl_Position = ftransform();
}