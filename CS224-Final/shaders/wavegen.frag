
const float height = 0.0;

varying vec3 normal;
varying vec4 pos;

void main(void)
{
    float depth = height - pos.y; 
    float n_z = dot(vec3(0, 1, 0), normal);

    gl_FragColor = vec4(depth, n_z, 0, 1);
    
    if (depth < 0.0) {
        // Reject this sample
        gl_FragColor = vec4(0, 0, 0, 1);
    }   
}