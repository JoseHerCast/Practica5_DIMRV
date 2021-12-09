#include <vector>
#include <string>
#include <stdio.h>
#include <SDL_opengl.h>
#include <math.h>

/*NOTA:Por el momento, el cargador de modelos solo cuenta con las siguientes características.

-Carga modelos de manera correcta siempre y cuando esten formados por primitivas triangulos.

-A pesar que puede leer y almacenar las coordenadas de textura, no se implemento su reconocimiento en las
caras ni su carga en memoria y por consiguiente en el modelo.

-Se pueden implementar modelos de luz y material sin problemas ya que si se tomaron en cuenta las normales indicadas
en cada cara

*/


class obj3dmodel
{
    //Estos serán nuestros vertices
    struct vertex {
        double x;
        double y;
        double z;
    };
    //Estas serán nuestras coordenadas uv
    struct text {
        double u;
        double v;
    };
    //Cada cara estará formada por 3 vertices (triangulos)
    struct face {
        unsigned int v1, v2, v3;
        unsigned int n1, n2, n3;
    };
    //Declaramos los arreglos de vertices y de caras
    std::vector<vertex> vertices;
    std::vector<vertex> normales;
    std::vector<text> textura;
    std::vector<face> caras;

public:

    char colision;//tipo de modelo de colisión (caja o esfera)
    //Para el modelo de esfera
    GLfloat center[3] = { 0,0,0 };
    GLfloat radius = 0;
    //Para el modelo de caja
    GLfloat maxX=0;
    GLfloat maxY=0;
    GLfloat maxZ=0;
    GLfloat minX = 0;
    GLfloat minY = 0;
    GLfloat minZ = 0;

    //Función de lectura del archivo
    bool readfile(const char* filename);
    //Función de dibujo del modelo
    void solidDraw(GLfloat* ambient, GLfloat* diffuse, GLfloat* specular, GLfloat shininess, char colision);
    void wireDraw();
    //GLfloat* getCenter();
    //char getColision();
};