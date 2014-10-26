/* created by Arthur Sonzogni */

#include "Shader.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

// shader collection
map<string,Shader*> shaderMap;

// program collection
map<string, ShaderProgram*> shaderProgramMap;


// Lecture d'un fichier
bool getFileContents(const char *filename, vector<char>& buffer)
{
    //debug("chargement du fichier : %s",filename);
    ifstream file(filename, ios_base::binary);
    if (file)
    {
        file.seekg(0, ios_base::end);
        streamsize size = file.tellg();
        if (size > 0)
        {
            file.seekg(0, ios_base::beg);
            buffer.resize(static_cast<size_t>(size));
            file.read(&buffer[0], size);
        }
        buffer.push_back('\0');
        return true;
    }
    else
    {
        return false;
    }
}


// Shader loading from a file
Shader& Shader::loadFromFile(const std::string &filename, ShaderType::T type)
{
    // test if the shader is already loaded
    auto it = shaderMap.find(filename);
    if (it!=shaderMap.end())
        return *(it->second);

    // else, we load it
    Shader* s = new Shader();
    s->name = filename;
    

    // file loading
    vector<char> fileContent;
    if (!getFileContents(filename.c_str(),fileContent))
    {
        cout<<"[Error] file "<<filename<<" not found ." << endl;
        exit(EXIT_FAILURE);
    }

    // creation
    GLuint handle = glCreateShader(type);
    s->handle = handle;
    if(handle == 0)
    {
        cout<<"[Error] Impossible to create a blank shader."<<endl;
    }

    // code source assignation
    const char* shaderText(&fileContent[0]);
    glShaderSource(handle, 1, (const GLchar**)&shaderText, NULL);

    // compilation
    glCompileShader(handle);

    // compilation check
    GLint compile_status;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &compile_status);
    if(compile_status != GL_TRUE)
    {
        /* error text retreiving*/
        GLsizei logsize = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logsize);
         
        char* log = new char[logsize+1];
        glGetShaderInfoLog(handle, logsize, &logsize, log);
        //log[logsize]='\0';
         
        cout<<"[Error] compilation error: "<<filename<<endl;
        cout<<log<<endl;
        
        exit(EXIT_FAILURE);
    }
    else
    {
        cout<<"[Info] Shader "<<filename<<" compiled successfully"<<endl;
    }

    // the shader is recorded to the database
    shaderMap[filename]=s;

    return *s;
}

Shader::Shader() :
handle(0)
{
}

Shader::Shader(const Shader& shader) :
handle(shader.handle)
{
}


ShaderProgram::ShaderProgram() : 
    isLinked(false)
{
}

ShaderProgram& ShaderProgram::loadFromFile(const char* vertexShader, const char* fragmentShader)
{
    return ShaderProgram::loadFromShader(
                Shader::loadFromFile(vertexShader,ShaderType::Vertex),
                Shader::loadFromFile(fragmentShader,ShaderType::Fragment)
            );
}

ShaderProgram& ShaderProgram::loadFromFile(const char* vertexShader, const char* geometryShader, const char* fragmentShader)
{
    return ShaderProgram::loadFromShader(
                Shader::loadFromFile(vertexShader,ShaderType::Vertex),
                Shader::loadFromFile(geometryShader,ShaderType::Geometry),
                Shader::loadFromFile(fragmentShader,ShaderType::Fragment)
            );
}

ShaderProgram* ShaderProgram::createProgram(string name)
{
    // Check if shaderprogram exists
    auto it(shaderProgramMap.find(name));
    if (it != shaderProgramMap.end())
        return (it->second);

    // programme creation
    ShaderProgram* p = new ShaderProgram();
    p->handle = glCreateProgram();
    if (not p->handle)
    {
        cout<<"Impossible to create a blank program"<<endl;
        exit(EXIT_FAILURE);
    }
    p->name = name;
    shaderProgramMap[p->name] = p;
    
    return p;

}
void ShaderProgram::link(ShaderProgram* p)
{
    glLinkProgram(p->handle);
    GLint result;
    glGetProgramiv(p->handle,GL_LINK_STATUS, &result);
    if (result!=GL_TRUE)
    {
        cout<<"[Error] linkage error"<<endl;

        /* error text retreiving*/
        GLsizei logsize = 0;
        glGetProgramiv(p->handle, GL_INFO_LOG_LENGTH, &logsize);
         
        char* log = new char[logsize];
        glGetProgramInfoLog(p->handle, logsize, &logsize, log);
        //log[logsize]='\0';
         
        cout<<log<<endl;
    }
    p->isLinked = true;
}

ShaderProgram& ShaderProgram::loadFromShader(Shader& vertexShader, Shader& fragmentShader)
{
    string name=vertexShader.name+fragmentShader.name;
    ShaderProgram* p = createProgram(name);
    if (p->isLinked) return *p;

    // attachement.
    glAttachShader(p->handle, vertexShader.handle);
    glAttachShader(p->handle, fragmentShader.handle);

    link(p);

    return *p;
}

ShaderProgram& ShaderProgram::loadFromShader(Shader& vertexShader, Shader& geometryShader, Shader& fragmentShader)
{
    string name=vertexShader.name+fragmentShader.name+geometryShader.name;
    ShaderProgram* p = createProgram(name);
    if (p->isLinked) return *p;

    // attachement.
    glAttachShader(p->handle, vertexShader.handle);
    glAttachShader(p->handle, geometryShader.handle);
    glAttachShader(p->handle, fragmentShader.handle);

    link(p);

    return *p;
}


GLint ShaderProgram::uniform(const std::string &name)
{
    auto it = uniforms.find(name);
    if (it == uniforms.end())
    {
        // uniform that is not referenced
        GLint r = glGetUniformLocation(handle, name.c_str()); 
        if ( r == GL_INVALID_OPERATION || r < 0 )
            cout<<"[Error] uniform "<<name<<" doesn't exist in program"<<endl;
        // add it anyways
        uniforms[name] = r;

        return r;
    }
    else
        return it->second; 
}

GLint ShaderProgram::attribLocation(const std::string& name)
{
    GLint attrib = glGetAttribLocation(handle, name.c_str());
    if (attrib == GL_INVALID_OPERATION || attrib < 0 )
        cout<<"[Error] Attribute "<<name<<" doesn't exist in program"<<endl;

    return attrib;
}

void ShaderProgram::setAttribute(const std::string& name, GLint size, GLboolean normalized, GLsizei stride, GLuint offset, GLenum type)
{
    GLint loc = attribLocation(name);
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(
            loc,
            size,
            type,
            normalized,
            stride*sizeof(GLfloat),
            (void*)(offset*sizeof(GLfloat))
	);
}

ShaderProgram::ShaderProgram(const ShaderProgram& other)
{
    cout<<"[error] trying to copy a shaderProgram"<<endl;
}

void ShaderProgram::setUniform(const std::string& name,float x,float y,float z)
{
    glUniform3f(uniform(name), x, y, z);
}
void ShaderProgram::setUniform(const std::string& name, const vec3 & v)
{
    glUniform3fv(uniform(name), 1, value_ptr(v));
}
void ShaderProgram::setUniform(const std::string& name, const dvec3 & v)
{
    glUniform3dv(uniform(name), 1, value_ptr(v));
}
void ShaderProgram::setUniform(const std::string& name, const vec4 & v)
{
    glUniform4fv(uniform(name), 1, value_ptr(v));
}
void ShaderProgram::setUniform(const std::string& name, const dvec4 & v)
{
    glUniform4dv(uniform(name), 1, value_ptr(v));
}
void ShaderProgram::setUniform(const std::string& name, const dmat4 & m)
{
    glUniformMatrix4dv(uniform(name), 1, GL_FALSE, value_ptr(m));
}
void ShaderProgram::setUniform(const std::string& name, const mat4 & m)
{
    glUniformMatrix4fv(uniform(name), 1, GL_FALSE, value_ptr(m));
}
void ShaderProgram::setUniform(const std::string& name, const mat3 & m)
{
    glUniformMatrix3fv(uniform(name), 1, GL_FALSE, value_ptr(m));
}
void ShaderProgram::setUniform(const std::string& name, float val )
{
    glUniform1f(uniform(name), val);
}
void ShaderProgram::setUniform(const std::string& name, int val )
{
    glUniform1i(uniform(name), val);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(handle);
}


void ShaderProgram::use() const
{
    glUseProgram(handle);
}
void ShaderProgram::unuse() const
{
    glUseProgram(0);
}
