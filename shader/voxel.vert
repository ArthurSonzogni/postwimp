#version 330

in vec3 position;
in vec3 normal;
in ivec4 color;

uniform mat4 projection = mat4(1);
uniform mat4 model = mat4(1);
uniform mat4 view = mat4(1);

uniform vec4 lightPosition = vec4(0.0,10.0,0.0,0.0);

out vec4 fColor;
out vec3 fNormal;
out vec3 fLightDirection;
out vec3 fEyeDirection;

void main()
{
    fColor = color.abgr/256.0;

    vec4 pos = view * model * vec4(position,1.0);

    fNormal = normalize(vec3(view * model * vec4(normal,0.0)));
    fLightDirection = normalize(vec3( view * model * lightPosition - pos) );
    fEyeDirection = normalize(- vec3( pos ));
    

    gl_Position = projection * pos;
}
