#version 330

layout (location = 0) in vec3 position;

uniform mat4 projection = mat4(1);
uniform mat4 model = mat4(1);
uniform mat4 view = mat4(1);


out vec3 fPosition;
out vec4 fNormal;

void main()
{
    vec4 worldPos = view * model * vec4(position,1.0);

    fPosition = worldPos.xyz;
    fNormal =  view * model * vec4(0.0,0.0,1.0,0.0);

    gl_Position = projection * worldPos;
}
