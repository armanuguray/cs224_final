
uniform vec3 tl;
uniform vec3 tr;
uniform vec3 bl;
uniform vec3 br;

void main()
{
    vec3 world = mix(mix(tl, tr, gl_TexCoord[0].x),
                     mix(bl, br, gl_TexCoord[0].x),
                     gl_TexCoord[0].y);

    gl_FragColor = vec4(gl_TexCoord[0].xy, 0.0, 1.0);
//    gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
}
