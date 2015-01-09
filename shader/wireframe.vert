#version 130

in vec3 position;
in vec4 color;

uniform mat4 projection;
uniform mat4 view;
out vec4 fColor;

void main(void)
{
    fColor = color;
    gl_Position = projection * view * vec4(position,1.0); 
};
