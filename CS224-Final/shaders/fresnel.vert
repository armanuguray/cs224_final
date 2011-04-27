
const vec4 light = vec4(70, 50, 70, 1);

varying vec3 normal, view, lightDir;

void main(void)
{
    normal = gl_Normal;
    lightDir = normalize(light.xyz - gl_Vertex.xyz);
    view = normalize(gl_Vertex.xyz - (gl_ModelViewMatrixInverse * vec4(0, 0, 0, 1)).xyz);
    
	gl_Position = ftransform();
}
