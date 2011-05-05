uniform samplerCube cube;
varying vec3 n, view;

uniform float wp_max_amplitude;
uniform float heightmap_resolution;

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

const float SPECULAR_EXP = 24.0;
const float SPECULAR_COEFF = 0.8;

varying vec3 test;

void main(void)
{
    vec3 nNormal = normalize(n);

    float eta = 0.67;
    float cos = dot(view, nNormal);
    if (cos > 0.0)
    {
        // eta = 1.33;
        nNormal *= -1.0;
    }

    vec3 reflected = reflect(view, nNormal);
    vec4 reflectColor = textureCube(cube, reflected);
    vec4 refractColor = textureCube(cube, refract(view, nNormal, eta));

    const float R_0 = 0.4;
    float fresnel = R_0 + (1.0 - R_0) * pow((1.0 - dot(-view, nNormal)), 5.0);
    fresnel = max(0.0, min(fresnel, 1.0));

    gl_FragColor = mix(refractColor, reflectColor, fresnel);
    //gl_FragColor = mix(vec4(1.0, 0.0, 0.0, 1.0), vec4(0.0, 0.0, 1.0, 1.0), fresnel);
    //gl_FragColor = vec4(nNormal, 1.0);
    //gl_FragColor = vec4(test, 1.0);
    // Trippy! Also it looks like the particles are spreading too much before subdividing ...

    // specular lighting
    float specular = max(dot(reflected, vec3(0.0, 1.0, 0.0)), 0.0);
    specular = pow(specular, SPECULAR_EXP) * SPECULAR_COEFF;

    gl_FragColor += vec4(specular, specular, specular, 1.0);
}
