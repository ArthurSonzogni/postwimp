/* created by Arthur Sonzogni */

#ifndef SHADER_F8X43H2W
#define SHADER_F8X43H2W

#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include "utils/glm.hpp"
#include <string>
#include <map>
#include <list>

class Shader;
class ShaderProgram;

namespace ShaderType
{
    enum T {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Geometry = GL_GEOMETRY_SHADER
    };
};

class Shader
{
    public:
        // Load Shader from a file
        static Shader& loadFromFile(const std::string &filename, ShaderType::T type);

        // provide opengl shader identifiant.
        GLuint getHandle() const;
        
        ~Shader();
    private:
        // forbidded constructor, use Shader::fromFile(filename)
        Shader();
        // forbidded copy constructor
        Shader(const Shader& shader);

        // opengl program identifiant
        GLuint handle;

        std::string name;

        friend class ShaderProgram;

};


class ShaderProgram
{
    public:
        // constructor
        static ShaderProgram& loadFromFile(const char* vertexShader, const char* fragmentShader);
        static ShaderProgram& loadFromFile(const char* vertexShader, const char* geometryShader, const char* fragmentShader);
        static ShaderProgram& loadFromShader(Shader& vertexShader, Shader& fragmentShader);
        static ShaderProgram& loadFromShader(Shader& vertexShader, Shader& geometryShader, Shader& fragmentShader);

        // bind the program
        void use() const;
        void unuse() const;

        // provide the opengl identifiant
        GLuint getHandle() const;

        // provide uniform location
        GLint uniform(const std::string& name);
        GLint operator[](const std::string& name);
        
        // same things with attributes
        GLint attribLocation(const std::string& name);
        void setAttribute(const std::string& name, GLint size, GLboolean normalized, GLsizei stride, GLuint offset, GLenum type = GL_FLOAT);

        // affect uniform
        void setUniform(const std::string& name, float x,float y,float z);
        void setUniform(const std::string& name, const glm::vec3 & v);
        void setUniform(const std::string& name, const glm::dvec3 & v);
        void setUniform(const std::string& name, const glm::vec4 & v);
        void setUniform(const std::string& name, const glm::dvec4 & v);
        void setUniform(const std::string& name, const glm::dmat4 & m);
        void setUniform(const std::string& name, const glm::mat4 & m);
        void setUniform(const std::string& name, const glm::mat3 & m);
        void setUniform(const std::string& name, float val );
        void setUniform(const std::string& name, int val );

        ~ShaderProgram();
    private:
        std::map<std::string, GLint> uniforms;
        std::map<std::string, GLint> attributes;

        // opengl identifiant
        GLuint handle;

        // forbidded constructor, use ShaderProgram::fromFile(filename)
        ShaderProgram();
        ShaderProgram(const ShaderProgram& other);

        // other
        static ShaderProgram* createProgram(std::string name);
        static void link(ShaderProgram* program);

        std::string name;
        bool isLinked;

};



#endif /* end of include guard: SHADER_F8X43H2W */
