#include "Axes.hpp"


Axes::Axes():
    program(ShaderProgram::loadFromFile(
        "shader/wireframe.vert",
        "shader/wireframe.frag")
    )
{
    initData();

    // vbo
    glGenBuffers( 1, &vbo );
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GL_Vertice), &data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // vao
    glGenVertexArrays( 1, &vao);
    glBindVertexArray(vao);

        // bind vbo
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // map vbo to shader attributes
        program.setAttribute("position", 3, GL_FALSE, 7, 0);
        program.setAttribute("color", 4, GL_FALSE, 7, 3);

    // vao end
    glBindVertexArray(0);

    std::cout << data.size() << std::endl;
    std::cout << vbo << std::endl;
    std::cout << vao << std::endl;
}

void Axes::initData()
{
    GL_Vertice v;

    // red
    v.color = glm::vec4(1.f,0.f,0.f,0.5f);
    v.position = glm::vec3(0.f,0.f,0.f);
    data.push_back(v);
    v.position = glm::vec3(v.color);
    data.push_back(v);

    // blue
    v.color = glm::vec4(0.f,1.f,0.f,0.5f);
    v.position = glm::vec3(0.f,0.f,0.f);
    data.push_back(v);
    v.position = glm::vec3(v.color);
    data.push_back(v);

    // green
    v.color = glm::vec4(0.f,0.f,1.f,0.5f);
    v.position = glm::vec3(0.f,0.f,0.f);
    data.push_back(v);
    v.position = glm::vec3(v.color);
    data.push_back(v);
}

void Axes::draw(const glm::mat4& projection, const glm::mat4& view)
{
    program.use();    

        // send uniforms
        program.setUniform("projection",projection);
        program.setUniform("view",view);


        glBindVertexArray(vao);
        
            glLineWidth(3.0);
            glDrawArrays(
                 GL_LINES,      // mode
                 0,            // offset
                 data.size()     // count
             );
            glLineWidth(1.0);

        glBindVertexArray(0);

    program.unuse();
}
