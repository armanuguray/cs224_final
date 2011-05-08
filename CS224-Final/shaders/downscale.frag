uniform sampler2D prev_texture;
uniform float new_size;

const float old_halfunit = 0.25 / new_size;
const float EPS = 0.001;

bool isBoundary(vec4 data) {
    return abs(data.z) > 0.0 || abs(data.w) > 0.0;
}

void main(void)
{
    vec2 loc = vec2((gl_FragCoord.x) / new_size,
                    (gl_FragCoord.y) / new_size);

    vec4 tl = texture2D(prev_texture, vec2(loc.x - old_halfunit, loc.y + old_halfunit));
    vec4 tr = texture2D(prev_texture, vec2(loc.x + old_halfunit, loc.y + old_halfunit));
    vec4 bl = texture2D(prev_texture, vec2(loc.x - old_halfunit, loc.y - old_halfunit));
    vec4 br = texture2D(prev_texture, vec2(loc.x + old_halfunit, loc.y - old_halfunit));

    float ct = 0.0;
    if (isBoundary(tl)) { ct++; }
    if (isBoundary(tr)) { ct++; }
    if (isBoundary(bl)) { ct++; }
    if (isBoundary(br)) { ct++; }

    vec2 dir = vec2(0, 0);
    if (ct > 0.0) {
        dir.x = tl.z + tr.z + bl.z + br.z;
	dir.y = tl.w + tr.w + bl.w + br.w;
    }

    gl_FragColor = vec4(ct,     
                        tl.y + tr.y + bl.y + br.y,     
                        dir.x, dir.y);

    // x: num of boundaries
    // y: indirect
    // z: dirx
    // w: diry
}
