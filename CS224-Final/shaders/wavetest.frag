
varying vec3 n;

uniform sampler2D texture;

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

void main()
{
    vec3 normal = normalize(n);
    vec3 world = mix(mix(tl, tr, gl_TexCoord[0].x),
                     mix(bl, br, gl_TexCoord[0].x),
                     gl_TexCoord[0].y);
    // TOOD: unlerp to get the heightmap tex coords
    // TDOO: sample the texture to get the height
    // TODO: do cool shit the height
    // TODO: except we're supposed to do that in the vertex shader whatever

    //gl_FragColor = vec4(gl_TexCoord[0].xy, 0.0, 1.0);
    gl_FragColor = texture2D(texture, gl_TexCoord[0].xy);
}
