#include "terreno.h"

void Terrain::generateMesh(GLuint width,GLuint depth)
{   
    //Almacenamos localmente los parametros del número de vertices en ancho y profundidad
    double inc = 1;
    this->width = width;
    this->depth = depth;
    int i, j,count;
    int aux[4];
    
    //Declaramos dos variables del tipo face y vertex contenidos en el archivo de cabecera
    face f;
    vertex v;
    //Mdeiante dos ciclos for recorremos la matriz de vertices formada por depth filas y width columnas de vertices
    for (i = 0; i < depth;i++) {
        for (j = 0; j < width; j++) {
            //Generamos los valores (x,y,z de los vertices que formaran nuestros triangulos)
            v.x = (float(i) / (width - 1) * 2.0f - 1.0);
            
            //De manera muy especifica estamos generando un plano inclinado a partir de la mitad del terreno
            //Para un terreno irregular, esta condicional puede ser sustituida simplemente por una asignación
            //pseudoaleatoria, esto nos daría valores de altura variados
            if (i > depth / 2 )
                v.y = inc;
            else
                v.y = 0;

            v.z = (float(j) / (depth - 1) * 2.0f - 1.0);
            this->vertices.push_back(v);
            //printf("\n( %lf, %lf)\n",v.x,v.z);
        }
        if (i > depth/2)
            inc++;
    }
    //Guardamos el valor de la máxima altura para poder calcular la pendiente posteriormente
    this->maxHeight = inc;
    //printf("\nNúmero de vertices %d\n",vertices.size());

    //Una vez generados los vertices, podemos obtener las caras o triangulos que formarán la malla
    //nuevamente debemos recorrer nuestra matriz de vertices.
    for (i = 0; i < depth - 1;i++) {
        for (j = 0; j < width - 1;j++) {
            aux[0] = (i * depth) + j;
            aux[1] = aux[0] + 1;
            aux[2] = ((i + 1) * depth) + j;
            aux[3] = aux[2] + 1;

            //printf("\n%d/%d/%d/%d\n", aux[0],aux[1],aux[2],aux[3]);
            //system("pause");


            //Primer triangulo
            f.v1 = aux[0];
            f.v2 = aux[2];
            f.v3 = aux[1];

            this->caras.push_back(f);

            //Segundo triangulo

            f.v1 = aux[1];
            f.v2 = aux[2];
            f.v3 = aux[3];

            this->caras.push_back(f);
        }
    }
    
    //NOTA: La generación de vertices y de caras podría hacerse en un solo recorrido de la matriz, sin embargo
    //para fines demostrativos se realizaron ambos procedimientos de manera independiente.

    //printf("\nNúmero de caras %d\n", caras.size());
}

void Terrain::solidDraw(GLfloat* ambient, GLfloat* diffuse, GLfloat* specular, GLfloat shininess)
{
    double u[3],v[3];
    GLfloat cruz[3];

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    glBegin(GL_TRIANGLES);

    //Dibujamos las caras que forman nuestro terreno
    for (int i = 0; i < caras.size(); i++)
    {
        //Almacenamos el valor de los vertices recorriendo cara por cara
        vertex v1 = vertices[caras[i].v1];
        vertex v2 = vertices[caras[i].v2];
        vertex v3 = vertices[caras[i].v3];

   /*     printf("v1= ( %.5lf, %.5lf, %.5lf )\n", v1.x, v1.y,v1.z);
        printf("v2= ( %.5lf, %.5lf, %.5lf )\n", v2.x, v2.y, v2.z);
        printf("v3= ( %.5lf, %.5lf, %.5lf )\n", v3.x, v3.y, v3.z);*/

        //Calculamos las normales para cada una de las caras

        u[0] = v2.x - v1.x;
        u[1] = v2.y - v1.y;
        u[2] = v2.z - v1.z;

        v[0] = v3.x - v1.x;
        v[1] = v3.y - v1.y;
        v[2] = v3.z - v1.z;

        //printf("u= ( %.lf, %.lf, %.lf )\n", u[0], u[1], u[2]);
        //printf("v= ( %.lf, %.lf, %.lf )\n\n", v[0], v[1], v[2]);

        cruz[0] = -(u[1] * v[2]) + (u[2]*v[1]);
        cruz[1]= -(u[2] * v[0]) + (u[0] * v[2]);
        cruz[2]= -(u[0] * v[1]) + (u[1] * v[0]);

        //printf("Normal= ( %.lf, %.lf, %.lf )\n",cruz[0], cruz[1], cruz[2]);
        //system("pause");
        
        glNormal3f(cruz[0],cruz[1],cruz[2]);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);

    }
    glEnd();
}

void Terrain::wireDraw()
{
    glBegin(GL_LINES);
    //Dibujamos las caras
    for (int i = 0; i < caras.size(); i++)
    {
        //Almacenamos el valor de los vertices recorriendo cara por cara
        vertex v1 = vertices[caras[i].v1];
        vertex v2 = vertices[caras[i].v2];
        vertex v3 = vertices[caras[i].v3];

        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);

        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);

        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
}
GLfloat Terrain::getHeight() {
    //Simplemente devolvemos el valor de la máxima altura que se genero para nuestro terreno
    return this->maxHeight;
}
