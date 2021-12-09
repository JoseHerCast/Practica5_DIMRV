#include <vector>
#include <string>
#include <stdio.h>
#include <SDL_opengl.h>


class Terrain
{
    //Estos serán nuestros vertices
    struct vertex {
        double x;
        double y;
        double z;
    };
    //Esta estructura permite almacenar tanto los vertices como normales de una cara
    struct face {
        unsigned int v1, v2, v3;
        unsigned int n1, n2, n3;
    };
    //Declaramos los arreglos de vertices, caras y normales
    std::vector<vertex> vertices;
    std::vector<vertex> normales;
    std::vector<face> caras;
    //Estas variables contendrán los parámteros de tamaño de nuestro terreno (en número de vertices).
    GLuint width;
    GLuint depth;
    //Esta variable almacena la máxima altura que tiene el terreno
    GLdouble maxHeight;

public:
    //Función para generar la malla  de nuestro terreno
    void generateMesh(GLuint width,GLuint depth);
    //Función de dibujo del terreno en forma solida, recibe el material que queremos que tenga
    void solidDraw(GLfloat* ambient, GLfloat* diffuse, GLfloat* specular, GLfloat shininess);
    //Función de dibujo del terreno en alambrada
    void wireDraw();
    //Devuelve el valor de altura del terreno
    GLfloat getHeight();
};