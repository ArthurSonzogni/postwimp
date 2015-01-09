#include <vector>
#include "ShaderLib.hpp"
#include <GL/glew.h>
#include <iostream>

class Axes
{
    public:
        Axes();
        void draw(const glm::mat4& projection, const glm::mat4& view);
    private:
        ShaderProgram& program;

    struct GL_Vertice
    {
        glm::vec3 position;
        glm::vec4 color;
    };
    std::vector<GL_Vertice> data;
    void initData();

    GLuint  vao,vbo;

};
