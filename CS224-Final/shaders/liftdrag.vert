uniform sampler2D heightmap;
uniform sampler2D velocitymap;

uniform float wp_max_amplitude;
uniform float wave_speed;

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

uniform float Cd; // drag coefficient
uniform float Cl; // lift coefficient
uniform float ro; // water density
uniform float Atotal; // total face area
uniform float epsilon; // user defined epsilon value that defines the dependency of face area on direction of motion. ([0, 1])

uniform mat4 ctm; // composite transformation matrix (object -> world)

// outputs
varying vec3 F_lift;
varying vec3 F_drag;

// computes fraction of the are of triangle that lies underwater
float fraction_in_water(float waterlevel, float y0, float y1, float y2)
{
    // bubble sort the y coordinates such that x is the smallest and z is the greatest
    float temp;
    if (y1 < y0) {
        temp = y1;
        y1 = y0;
        y0 = temp;
    }
    if (y2 < y1) {
        temp = y2;
        y2 = y1;
        y1 = temp;
    }
    if (y1 < y0) {
        temp = y1;
        y1 = y0;
        y0 = temp;
    }
    
    // use cem yuksel's algorithm to find the fraction
    float fraction = 1.0;                                   // assume fully in water
    if (waterlevel < y0) return 0.0;                        // not in water
    float h01 = y0 - y1;
    float h02 = y0 - y2;
    float h12 = y1 - y2;
    if ( waterlevel < y1 ) {                                // lower part of the triangle
        float h0w = y0 - waterlevel;
        fraction = ( h0w * h0w ) / ( h01 * h02 );
    } else if ( waterlevel < y2 ) {                         // higher part of the triangle
        float hw2 = waterlevel - y2;
        fraction = 1.0 - ( hw2 * hw2 ) / ( h12 * h02 );
    }                                                       // otherwise fully in water
    return fraction;
}

void main(void)
{
    gl_Position = ftransform();
    
    // store the corners in world space
    vec3 v0 = (ctm * vec4(gl_MultiTexCoord0.xyz, 1.0)).xyz;
    vec3 v1 = (ctm * vec4(gl_MultiTexCoord1.xyz, 1.0)).xyz;
    vec3 v2 = (ctm * vec4(gl_MultiTexCoord2.xyz, 1.0)).xyz;
    
    vec3 vcentroid = (v0 + v1 + v2)/3.0;
    
    vec2 lookup = vec2((vcentroid.x - htl.x) / (htr.x - htl.x),
                       (vcentroid.z - htr.z) / (hbr.z - htr.z));
    
    float surface = 0.0;
    vec3 water_velocity = vec3(0.0);
    if (lookup.x >= 0.0 && lookup.x <= 1.0 && lookup.y >= 0.0 && lookup.y <= 1.0) {
        surface = wp_max_amplitude * (2.0 * texture2D(heightmap, lookup).y - 1.0);
        vec2 v = wave_speed * (2.0 * texture2D(velocitymap, lookup).xy) - vec2(1.0);
        water_velocity = vec3(v.x, 0.0, v.y);
    }

 //   water_velocity = vec3(0.06, 0.0, 0.06);
    
    // get the normal in world space
    vec3 N = normalize((ctm * vec4(gl_Normal.xyz, 0.0)).xyz);
    // get the velocity
    vec3 U = gl_MultiTexCoord3.xyz - (9.0 * water_velocity);
    
    float Af = Atotal * fraction_in_water(surface, v0.y, v1.y, v2.y);

    if(Af < 1e-6 || length(U) < 1e-6) {                                                    // if not submerged, then the forces are 0
        F_lift = F_drag = vec3(0.0, 0.0, 0.0);
    } else {
        float dot_ = dot(N,normalize(U));
        float A = (dot_ * epsilon + 1.0 - epsilon)*Af;     // effective area
        float factor = -0.5 * ro * A * length(U);
        if (dot_ >= 0.0)
            F_drag = factor * Cd * U;
        else F_drag = vec3(0.0, 0.0, 0.0);
        // if the direction of motion is aligned with the surface normal, then there is no lift
        if (abs(dot_ - 1.0) < 1e-4 || abs(dot_ + 1.0) < 1e-4)
            F_lift = vec3(0.0, 0.0, 0.0);
        else {
            F_lift = vec3(0.0, 0.0, 0.0);//factor * Cl * cross(U, normalize(cross(N, U)));
        }
    }
}


