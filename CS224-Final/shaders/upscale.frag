uniform sampler2D prev_texture;
uniform sampler2D downscale_texture;
uniform sampler2D downscale_prev_texture;

uniform float new_size;

float old_halfunit = 0.5 / new_size;
const float EPS = 0.0001;

bool isBoundary(vec4 data) {
    return abs(data.z) > 0.0 || abs(data.w) > 0.0;
}

void main(void)
{
    vec2 loc = vec2((gl_FragCoord.x) / new_size,
                    (gl_FragCoord.y) / new_size);
    vec4 prev = texture2D(prev_texture, loc);
    vec4 downscale = texture2D(downscale_texture, loc);
    vec4 downscale_prev = texture2D(downscale_prev_texture, loc);

    float indirect = 0.0;
    if (isBoundary(downscale)) {
        indirect = (prev.y + downscale.y) / downscale.x;
    }

    gl_FragColor = vec4(downscale.x, indirect,
                        (prev.z + downscale.z) / 2.0, (prev.w + downscale.w) / 2.0);
}
