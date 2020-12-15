/* header file for objloader class -
   resposible for loading .obj models */

#ifndef OBJLOADER_H
#define OBJLOADER_H
#include <GL/gl.h>
#include <string>
#include <vector>
#include <QVector2D>
#include <QVector3D>
#include <QFile>
#include <QDebug>

class objloader
{

    struct face
    {
        struct vertex {
            GLuint v_i; // vertex index
            GLuint vt_i; // texture vertex index
            GLuint vn_i; // normal vertex index
        };

        vertex v[3]; // triangle vertexes

        face(vertex v1,vertex v2,vertex v3) // polygone
        {
            v[0]=v1;
            v[1]=v2;
            v[2]=v3;
        }
    };

    std::vector<std::string> coord;
    std::vector<QVector3D> vertex;
    std::vector<QVector2D> uvs;
    std::vector<QVector3D> normals;
    std::vector<face> faces;

    objloader(){}
    objloader(const objloader &)  = delete;
    objloader(const objloader &&) = delete;
    objloader& operator=(const objloader &)  = delete;
    objloader& operator=(const objloader &&) = delete;
public:
    static objloader& Instance()
    {
        static objloader theSingleInstance;
        return theSingleInstance;
    }
    GLuint load(const QString &filename);
    GLuint draw(GLfloat offset = 0);

};

#endif // OBJLOADER_H

























