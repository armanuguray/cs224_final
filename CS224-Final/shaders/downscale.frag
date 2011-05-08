uniform sampler2D prev_texture;
uniform float new_size;

const float old_halfunit = 0.5 / new_size;
const float EPS = 0.0001;

bool isBoundary(vec4 data) {
    return !(data.z < EPS && data.w < EPS);
}

void main(void)
{
    vec2 loc = vec2((gl_FragCoord.x) / new_size,
                    (gl_FragCoord.y) / new_size);

    vec4 tl = texture2D(prev_texture, vec2(loc.x - old_halfunit, loc.y + old_halfunit));
    vec4 tr = texture2D(prev_texture, vec2(loc.x + old_halfunit, loc.y + old_halfunit));
    vec4 bl = texture2D(prev_texture, vec2(loc.x - old_halfunit, loc.y - old_halfunit));
    vec4 br = texture2D(prev_texture, vec2(loc.x + old_halfunit, loc.y - old_halfunit));

    vec2 dir = vec2(0, 0);
    if (isBoundary(tl) || isBoundary(tr) || isBoundary(bl) || isBoundary(br)) {
        dir.x = tl.z + tr.z + bl.z + br.z;
	dir.y = tl.w + tr.w + bl.w + br.w;
    }

    gl_FragColor = vec4(tl.x + tr.x + bl.x + br.x,     
                        tl.y + tr.y + bl.y + br.y,     
                        dir.x, dir.y);
}