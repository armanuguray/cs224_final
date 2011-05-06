
const float surface = 0.0; // this is going to change later and be looked up from the height map

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
            depth = pos.y; // negative
        // if normal is pointing down
        else if (n.y < 0.0) 
            depth = -pos.y; // positive
        gl_FragColor = vec4(1.0,0.0,0.0,1.0);
    } else gl_FragColor = vec4(0.0,1.0,0.0,1.0);
    
    // TODO: additively blend
    //gl_FragColor += vec4(depth, 0.0, 0.0, 1.0);
}