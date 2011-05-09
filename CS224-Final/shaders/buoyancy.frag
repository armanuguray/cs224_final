
uniform sampler2D heightmap;
uniform float wp_max_amplitude;

// Corners of the projected grid
uniform vec3 tl;
uniform vec3 tr;
uniform vec3 bl;
uniform vec3 br;

// Corners of the heightmap
uniform vec3 htl;
uniform vec3 htr;
uniform vec3 hbl;
uniform vec3 hbr;

varying vec3 normal;
varying vec4 pos;

const float epsilon = 1e-6;

void main(void)
{
    vec2 lookup = vec2((pos.x - htl.x) / (htr.x - htl.x),
                       (pos.z - htr.z) / (hbr.z - htr.z));
    
    float surface = 0.0;
    if (lookup.x >= 0.0 && lookup.x <= 1.0 && lookup.y >= 0.0 && lookup.y <= 1.0)
        surface = wp_max_amplitude * (2.0 * texture2D(heightmap, lookup).y - 1.0);
    
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
