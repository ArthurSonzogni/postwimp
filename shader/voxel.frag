#version 330
in vec4 fColor;
in vec3 fNormal;
in vec3 fLightDirection;
in vec3 fEyeDirection;

uniform float eta = 128.0;

void main()
{
    vec3 n = normalize(fNormal);
    vec3 e = normalize(fEyeDirection);
    vec3 l = normalize(fLightDirection);
    vec3 r = normalize(reflect(-e,n));

    float ambient = 0.3;
    float diffus = max(dot(n,l),0.0) * 0.5;
    float specular = pow(max(dot(r,l),0.0),eta) * 2.0 * fColor.a;

    gl_FragColor.a = 0.5;
    gl_FragColor.rgb = fColor.rgb * (ambient+diffus+specular);
} 
