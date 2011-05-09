
uniform float wp_amplitude;
uniform float max_amplitude;

uniform vec2 wp_direction;

void main()
{
    gl_FragData[0] = vec4(wp_amplitude / max_amplitude, -wp_amplitude / max_amplitude, 0.0, 1.0);

    gl_FragData[1].xy = clamp(wp_direction, 0.0, 1.0);
    gl_FragData[1].zw = clamp(-wp_direction, 0.0, 1.0);


  //    gl_FragData[1].xy = vec2(.5) + .5 * wp_direction;
//    gl_FragData[1] = vec4(wp_direction, -wp_direction);
}
