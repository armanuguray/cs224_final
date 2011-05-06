
const float surface = 0.0; // this is going to change later and be looked up from the height map
uniform float max_abs_height; // maximum absolute distance an object can have from the water

varying vec3 normal;
varying vec4 pos;

const float epsilon = 1e-6;

void main(void)
{
    // this is the final volume value for this fragment
    float depth = 0.0; 
    vec3 n = normalize(normal);
    // if underwater
    if (pos.y < surface) {
        // if normal is pointing up
        if (n.y > 0.0) 
            // negative
            gl_FragColor = vec4(pos.y, 0.0, 0.0, 1.0);
        // if normal is pointing down
        else if (n.y < 0.0) 
            // positive
            gl_FragColor = vec4(-pos.y, 0.0, 0.0, 1.0);
    } else gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}