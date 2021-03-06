varying vec3 normal;
varying vec3 vel;
varying vec4 pos;

uniform sampler2D overview;

const float height = 0.0;
const float bias = 1.0;
const float texture_size = 16.0;
const float AREA = 4.0;
const float dt = 1.0 / 30.0;

void main(void)
{
    float direct = 0.0;
    float indirect = 0.0;

    float face_depth = height - pos.y;

    vec4 sample = texture2D(overview, vec2(gl_FragCoord.x / texture_size, gl_FragCoord.y / texture_size));
    float depth = sample.x;
    float n_z = sample.y;

    if (n_z > 0.0) {
        if (face_depth > depth + bias) {
	    // indirect wave effect
	    indirect = AREA * dt * dot(vel, normal);
	    // gl_FragColor = vec4(1, 0, 0, 1);
	} else {
	    // direct wave effect
	    direct = AREA * dt * dot(vel, normal);
	    // gl_FragColor = vec4(0, 1, 0, 1);
	}	
    } else {
        // indirect wave effect
        indirect = AREA * dt * dot(vel, normal);
	// gl_FragColor = vec4(0, 0, 1, 1);
    }

    // TESTING ONLY
    // if (direct != 0.0) { direct = direct/2.0 + 0.5; }
    // if (indirect != 0.0) { indirect = indirect/2.0 + 0.5; }

    gl_FragColor = vec4(direct, indirect, 0.0, 1.0);
//    gl_FragColor = vec4(dot(vel, normal), 0.0, 0.0, 1.0);

    if (face_depth < 0.0) {
        // Reject this sample
	gl_FragColor = vec4(0, 0, 0, 1);
    }
}