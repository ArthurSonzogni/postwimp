/* created by Arthur Sonzogni */

#include "ModelObj.hpp"


#include <iostream>
#include <fstream>
#include <vector>
#include <clocale>
#include <cstdio>
#include <string>
#include "Shader.hpp"
#include "utils/glError.hpp"

using namespace std;

ModelObj::ModelObj(const char * filename, ShaderProgram& _shader, const char * textureName):
    hasTexture(true),
    obj(filename),
    shader(_shader),
    texture(NULL)
{
    texture = new Texture(Texture::loadFromFile(textureName));
    shader.use();

    // vao allocation
    glGenVertexArrays(1,&vao);

    glCheckError(__FILE__,__LINE__);

    // vao filling
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,obj.getVBO());
    shader.setAttribute("position", 3, GL_FALSE, 8, 0);
    shader.setAttribute("normal"  , 3, GL_FALSE, 8, 3);
    shader.setAttribute("texCoord", 2, GL_FALSE, 8, 6);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    glCheckError(__FILE__,__LINE__);
}

ModelObj::ModelObj(const char * filename, ShaderProgram& _shader):
    hasTexture(false),
    obj(filename),
    shader(_shader),
    texture(NULL)
{
    texture = NULL;
    shader.use();
    // vao allocation
    glGenVertexArrays(1,&vao);

    glCheckError(__FILE__,__LINE__);

    // vao filling
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,obj.getVBO());
    shader.setAttribute("position", 3, GL_FALSE, 8, 0);
    shader.setAttribute("normal"  , 3, GL_FALSE, 8, 3);
    shader.setAttribute("texCoord", 2, GL_FALSE, 8, 6);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    glCheckError(__FILE__,__LINE__);
}

ModelObj::ModelObj(const char * filename, ShaderProgram& _shader, Texture& _texture):
    hasTexture(true),
    obj(filename),
    shader(_shader),
    texture(NULL)
{
	texture = new Texture(_texture);
    shader.use();
    // vao allocation
    glGenVertexArrays(1,&vao);

    glCheckError(__FILE__,__LINE__);

    // vao filling
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,obj.getVBO());
    shader.setAttribute("position", 3, GL_FALSE, 8, 0);
    shader.setAttribute("normal"  , 3, GL_FALSE, 8, 3);
    shader.setAttribute("texCoord", 2, GL_FALSE, 8, 6);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    glCheckError(__FILE__,__LINE__);
}

ModelObj::~ModelObj()
{
    glDeleteVertexArrays(1,&vao);
}


void ModelObj::draw()
{
    shader.use();

    if (hasTexture)
    {
        texture->bind(GL_TEXTURE0);
        shader.setUniform("texture0",0);
    }

    glBindVertexArray(vao);

     
	glDrawArrays(
		GL_TRIANGLES,      // mode
        0,                 // first
		obj.getNbFace()*3           // count
	);

    glBindVertexArray(0);
    shader.unuse();
}

ShaderProgram& ModelObj::getShader()
{
    return shader;
}

Texture& ModelObj::getTexture()
{
	return *texture;
}

Obj::Dimension ModelObj::getDimension()
{
    return obj.getDimension();
}

void ModelObj::loadTexture(const char *fileName)
{
	if(texture) delete texture;
	texture = new Texture(Texture::loadFromFile(fileName));
}
