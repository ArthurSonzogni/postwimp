/* created by Arthur Sonzogni */

#include "Obj.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <clocale>
#include <cstdio>
#include <string>

using namespace std;


struct GLfloat3
{
	GLfloat x,y,z;
	GLfloat3(GLfloat a, GLfloat b, GLfloat c):x(a),y(b),z(c){};
};

struct GLfloat2
{
	GLfloat x,y;
	GLfloat2(GLfloat a, GLfloat b):x(a),y(b){};
};

struct face
{
	GLuint n[9];
};

Obj::Obj(const char * filename)
{
    glCheckError(__FILE__,__LINE__);
    vbo=0;

	ifstream file;
    
    // file open
	file.open(filename);
    if (not file.is_open())
    {
        cerr<<"[Error] the file "<<filename<<"doesn't exist"<<endl;
        exit(EXIT_FAILURE);
    }

	vector<GLfloat3> vertex;
	vector<GLfloat3> normals;
	vector<GLfloat2> texture;
    vector<GLfloat> vertexArray;
	vector<face> faces;

    // file parsing
	string identifier;
	while(file>>identifier)
	{
		if (identifier=="v")
		{
			double x,y,z;
			file>>x>>y>>z;
			vertex.push_back(GLfloat3(x,y,z));
		}
		else if (identifier=="vn")
		{
			double x,y,z;
			file>>x>>y>>z;
			normals.push_back(GLfloat3(x,y,z));
		}
        else if (identifier=="vt")
        {
            double x,y;
            file>>x>>y;
            texture.push_back(GLfloat2(x,y));
        }
		else if (identifier=="f")
		{
			getline(file,identifier);
			setlocale(LC_ALL, "en_US.utf8");
			face f;
			sscanf(identifier.c_str(),"%d/%d/%d %d/%d/%d %d/%d/%d",
					&f.n[0],
					&f.n[1],
					&f.n[2],
					&f.n[3],
					&f.n[4],
					&f.n[5],
					&f.n[6],
					&f.n[7],
					&f.n[8]
			);
			for(int i=0;i<9;++i)
				f.n[i]--;
			faces.push_back(f);
		}
	}
	//cout<<vertex.size()<<endl;
	//cout<<normals.size()<<endl;
	//cout<<faces.size()<<endl;
    faceNb = faces.size();

    // vertexArray construction
	for(unsigned int i=0;i<faceNb;++i)
	{
        int f=0;
		for(int a=0;a<3;++a)
		{
            auto fv = faces[i].n[f++];
            auto ft = faces[i].n[f++];
            auto fn = faces[i].n[f++];

			vertexArray.push_back(vertex[fv].x);
			vertexArray.push_back(vertex[fv].y);
			vertexArray.push_back(vertex[fv].z);

			vertexArray.push_back(normals[fn].x);
			vertexArray.push_back(normals[fn].y);
			vertexArray.push_back(normals[fn].z);

			vertexArray.push_back(texture[ft].x);
			vertexArray.push_back(-texture[ft].y);
		}
	}

	file.close();

    // vbo allocation
    glGenBuffers( 1, &vbo );

    // vbo filling
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertexArray.size(), &vertexArray[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glCheckError(__FILE__,__LINE__);


    // computing dimension;
    dimension.xmin = vertexArray[0];
    dimension.ymin = vertexArray[1];
    dimension.zmin = vertexArray[2];
    dimension.xmax = vertexArray[0];
    dimension.ymax = vertexArray[1];
    dimension.zmax = vertexArray[2];
	for(unsigned int i=0;i<vertexArray.size();i+=8)
    {
        dimension.xmin = min(dimension.xmin,vertexArray[0+i]);
        dimension.ymin = min(dimension.ymin,vertexArray[1+i]);
        dimension.zmin = min(dimension.zmin,vertexArray[2+i]);
        dimension.xmax = max(dimension.xmax,vertexArray[0+i]);
        dimension.ymax = max(dimension.ymax,vertexArray[1+i]);
        dimension.zmax = max(dimension.zmax,vertexArray[2+i]);
    }
}

Obj::~Obj()
{
    glDeleteBuffers(1,&vbo);
}

GLuint Obj::getVBO() const
{
    return vbo;
}

unsigned int Obj::getNbFace() const
{
    return faceNb;
}

Obj::Dimension Obj::getDimension()
{
    return dimension;
}
