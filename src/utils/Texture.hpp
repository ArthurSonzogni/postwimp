// created by Arthur Sonzogni

#ifndef __TEXTUREMANAGER_H__
#define __TEXTUREMANAGER_H__

#include <GL/glew.h>
#include "utils/Singleton.hpp"
#include <string>

class Texture
{
	public:
		static Texture& loadFromFile(std::string filename);
		void bind(GLuint activetexture = GL_TEXTURE0);
	private:
        Texture(GLuint id,int width,int height); // private use Texture::loadFromFile(filename)
        GLuint id;
        int width;
        int height;
};


#endif
