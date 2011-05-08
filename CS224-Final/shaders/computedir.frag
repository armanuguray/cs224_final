const float size = 16.0;
const float unit = 1.0 / size;

uniform sampler2D silhouette;
uniform sampler2D effects;

bool empty(vec4 shadow) {
    return shadow.x == 0.0 && shadow.y == 0.0;
}

void main(void)
{
    vec2 coords = vec2(gl_FragCoord) / size;
    vec4 effects_data = texture2D(effects, coords);

    vec4 curr_pos = texture2D(silhouette, coords);

    vec2 boundary_dir = vec2(0, 0);
    int ct = 0;
    // This is a boundary point! Compute some directions
    vec4 left_color = texture2D(silhouette, vec2(coords.x - unit, coords.y));
    if (empty(left_color)) {
        boundary_dir.x -= 1.0;
	ct += 1;
    }

    vec4 right_color = texture2D(silhouette, vec2(coords.x + unit, coords.y));
    if (empty(right_color)) {
        boundary_dir.x += 1.0;
	ct += 1;
    }

    vec4 top_color = texture2D(silhouette, vec2(coords.x, coords.y + unit));
    if (empty(top_color)) {
        boundary_dir.y += 1.0;
	ct += 1;
    }

    vec4 down_color = texture2D(silhouette, vec2(coords.x, coords.y - unit));
    if (empty(down_color)) {
        boundary_dir.y -= 1.0;
	ct += 1;
    }

    if (empty(curr_pos) && ct < 4) {
        // This is a boundary point!

        if (curr_pos.y > 0.0) {
            // This means N_z >= 0
    	    boundary_dir *= -1.0;
        }

//    gl_FragColor = vec4(coords.x, coords.y, 0, 1);
//    gl_FragColor = vec4(boundary_dir.x, boundary_dir.y, 0, 1);
      gl_FragColor = vec4(0, 0, boundary_dir.x, boundary_dir.y);

    } else {
        // This is not a boundary point!
	gl_FragColor = vec4(0, 0, 0, 0);
    }

    gl_FragColor.x = effects_data.x;
    gl_FragColor.y = effects_data.y;

    // x: direct
    // y: indirect
    // z: x-dir
    // w: y-dir

    // gl_FragColor = vec4(boundary_dir.x, boundary_dir.y, 0, 1);
}
