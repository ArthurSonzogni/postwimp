#version 130

in vec3 position;
in vec3 normal;
in vec2 texCoord;

uniform mat4 projection = mat4(1);
uniform mat4 view = mat4(1);

uniform vec4 lightPosition = vec4(0.0,10.0,0.0,0.0);
uniform float size = 1.0;
uniform vec3  pos = vec3(0.0,0.0,0.0);
uniform vec4 color = vec4(1.0);

out vec4 fColor;
out vec3 fNormal;
out vec3 fLightDirection;
out vec3 fEyeDirection;

void main()
{
    fColor = color;
    vec4 pos = view * vec4(size * position + pos,1.0);
    fNormal = normalize(vec3(view * vec4(normal,0.0)));
    fLightDirection = normalize(vec3( view * lightPosition - pos) );
    fEyeDirection = normalize(- vec3( pos ));
    
    gl_Position = projection * pos;
}
