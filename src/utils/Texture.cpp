#include "Texture.hpp"
#include "stb_image.h"
#include <iostream>
#include <cstdlib>
#include <map>
#include "glError.hpp"

using namespace std;

std::map<std::string, Texture*> textures;

void Texture::bind(GLuint activetexture)
{
    glActiveTexture(activetexture);
    glBindTexture(GL_TEXTURE_2D, id);
}

unsigned char * imageLoad(
		const std::string &filename,
		int& width,
		int& height,
		int &comp)
{
	cout<<"imageLoad[\""<<filename<<"\");"<<endl;
	unsigned char *data;

	FILE *file = fopen(filename.c_str(), "rb");
	if (!file) {
        cerr<<"[Error] file "<<filename<<" doesn't exists"<<endl;
        exit(EXIT_FAILURE);
    }

	data = stbi_load_from_file(file, &width, &height, &comp, 0);

	cout<<"imageLoad[\""<<filename<<"\","<<(void*)data<<","<<width<<","<<height<<","<<comp<<"]"<<endl;
	fclose(file);

	return data;
}


Texture& Texture::loadFromFile(string filename)
{
    glCheckError(__FILE__,__LINE__);

	auto it(textures.find(filename));
	if (it != textures.end())
		return *(it->second);

	GLuint id;
	int width, height, comp;

	unsigned char * data=imageLoad(filename,width,height,comp);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
    if (comp == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    else if (comp == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else if (comp == 1)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


	free(data);
    glCheckError(__FILE__,__LINE__);
    glBindTexture(GL_TEXTURE_2D,GL_NONE);

    Texture* texture = new Texture(id,width,height);
    textures[filename] = texture;

	return *texture;
}

Texture::Texture(GLuint _id, int _width, int _height):
    id(_id),
    width(_width),
    height(_height)
{
}
