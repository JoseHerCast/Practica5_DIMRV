#include "model.h"

/*NOTA:Por el momento, el cargador de modelos solo cuenta con las siguientes características.

-Carga modelos de manera correcta siempre y cuando esten formados por primitivas triangulos.

-A pesar que puede leer y almacenar las coordenadas de textura, no se implemento su reconocimiento en las
caras ni su carga en memoria y por consiguiente en el modelo.

-Se pueden implementar modelos de luz y material sin problemas ya que si se tomaron en cuenta las normales indicadas
en cada cara

*/

double longitud(double cx, double cy, double cz, double vx, double vy, double vz) {

    return sqrt(pow(cx-vx,2)+pow(cy - vy,2)+pow(cz - vz,2));
}

bool obj3dmodel::readfile(const char* filename)
{   
    char linea[55];

    //Aqui leemos el archivo OBJ
    FILE* obj = fopen(filename, "r");
    if (obj == NULL) {
        printf("\nImpossible to open the file %s\n",filename);
        return false;
    }
    
    char *auxv;
    //Leemos linea a linea
    while (fgets(linea, sizeof(linea), obj)) {
        switch (linea[0])
        {
        case 'V':
        case 'v':
            //Si nuestra linea es Vt (coordenada de textura)
            if (linea[1]=='t') {
                text t;
                //strtok(); le el buffer de entrada del fichero hasta que el caracter concuerda con el proporcionado
                auxv = strtok(linea, " "); //Descartamos el primer espacio que es en donde comienzan los valores númericos
                //Obtenemos los valores posteriores separados por un espacio
                auxv = strtok(NULL, " "); t.u = atof(auxv);
                auxv = strtok(NULL, " "); t.v = atof(auxv);
                //Añadimos el valor leido a nuestro arreglo de vertices
                this->textura.push_back(t);
                break;
            }
            //Si nuestra linea es Vn (Normal)
            else if (linea[1]=='n') {
                vertex v;
                //strtok(); le el buffer de entrada del fichero hasta que el caracter concuerda con el proporcionado
                auxv = strtok(linea, " "); //Descartamos el primer espacio que es en donde comienzan los valores númericos
                //Obtenemos los valores posteriores separados por un espacio
                auxv = strtok(NULL, " "); v.x = atof(auxv);
                auxv = strtok(NULL, " "); v.y = atof(auxv);
                auxv = strtok(NULL, " "); v.z = atof(auxv);
                //Añadimos el valor leido a nuestro arreglo de vertices
                this->normales.push_back(v);
                break;
            }
            //Si nuestra linea es un simple vertice
            else {
                vertex v;
                //strtok(); le el buffer de entrada del fichero hasta que el caracter concuerda con el proporcionado
                auxv = strtok(linea, " "); //Descartamos el primer espacio que es en donde comienzan los valores númericos
                //Obtenemos los valores posteriores separados por un espacio
                auxv = strtok(NULL, " "); v.x = atof(auxv);
                auxv = strtok(NULL, " "); v.y = atof(auxv);
                auxv = strtok(NULL, " "); v.z = atof(auxv);
                //Añadimos el valor leido a nuestro arreglo de vertices
                this->vertices.push_back(v);
            }
            break;
        case 'F':
        case 'f':
            //Si nuestra linea es una cara (Face)
            face f;
            auxv = strtok(linea, " "); //Descartamos el primer espacio que es en donde comienzan los valores númericos
            //Obtenemos los valores posteriores separados por un espacio
            auxv = strtok(NULL, "/"); f.v1 = atoi(auxv);
            auxv = strtok(NULL, "/");
            auxv = strtok(NULL, " "); f.n1 = atoi(auxv);
            auxv = strtok(NULL, "/"); f.v2 = atoi(auxv);
            auxv = strtok(NULL, "/");
            auxv = strtok(NULL, " "); f.n2 = atoi(auxv);
            auxv = strtok(NULL, "/"); f.v3 = atoi(auxv);
            auxv = strtok(NULL, "/");
            auxv = strtok(NULL, " "); f.n3 = atoi(auxv);
            //Añadimos el valor leido a nuestro arreglo de caras
            this->caras.push_back(f);

            break;
        default:
            break;
        }
    }

    printf("\nNúmero de vértices= %d\n",vertices.size());

    //Aqui leemos el archivo MTL
    char* mtlname = (char*)malloc(strlen(filename) * sizeof(char));
    strcpy(mtlname, filename);
    mtlname[strlen(mtlname) - 3] = NULL;
    strcat(mtlname, "mtl");


    FILE* mtl = fopen(mtlname, "r");
    if (mtl == NULL) {
        printf("\nImpossible to open the file %s\n", mtlname);
        return false;
    }

    while (fgets(linea, sizeof(linea), mtl)) {
        printf("\n%s\n", linea);
    }

    fclose(mtl);
    fclose(obj);
}

void obj3dmodel::solidDraw(GLfloat *ambient, GLfloat* diffuse, GLfloat* specular, GLfloat shininess, char colision)
{
    int i, j;
    this->colision = colision;

    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    glBegin(GL_TRIANGLES);
    //Dibujamos las caras
    for (i = 0; i < caras.size(); i++)
    {
        //Almacenamos el valor de los vertices recorriendo cara por cara
        vertex v1 = vertices[caras[i].v1 - 1];
        vertex v2 = vertices[caras[i].v2 - 1];
        vertex v3 = vertices[caras[i].v3 - 1];

        //Almacenamos el valor de las normales recorriendo cara por cara
        vertex a1 = normales[caras[i].n1 - 1];
        vertex a2 = normales[caras[i].n2 - 1];
        vertex a3 = normales[caras[i].n3 - 1];
        //"indexamos" nuestros valores, simplemente es una forma sencilla de utilizar GLfloat
        GLfloat n1[3] = {a1.x,a1.y,a1.z};
        GLfloat n2[3]={ a2.x,a2.y,a2.z };
        GLfloat n3[3]= { a3.x,a3.y,a3.z };
        
        //Dibujamos con todo y normales
        glNormal3fv(n1);
        glVertex3f(v1.x, v1.y, v1.z);
        glNormal3fv(n2);
        glVertex3f(v2.x, v2.y, v2.z);
        glNormal3fv(n3);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();

    //Colisiones
    int vn = vertices.size();//Número de vertices del modelo
    if (colision == 's' || colision == 'S') {
        //Colision esfera
        double radio = 0, aux = 0;
        vertex centro = { 0,0,0 };//Nuestro centro en (0,0,0)

        double theta, phi;
        int meridianos = 20, paralelos = 20;

        GLfloat v1[] = { 0,0,0 };
        GLfloat v2[] = { 0,0,0 };
        GLfloat v3[] = { 0,0,0 };
        GLfloat v4[] = { 0,0,0 };
        GLfloat angulom, angulop;
        //Calculo del centro de la esfera
        for (i = 0; i < vn; i++) {
            centro.x += vertices[i].x;
            centro.y += vertices[i].y;
            centro.z += vertices[i].z;
        }
        //Centro final de nuestra esfera
        centro.x = centro.x / vn;
        centro.y = centro.y / vn;
        centro.z = centro.z / vn;

        this->center[0] = centro.x;
        this->center[1] = centro.y;
        this->center[2] = centro.z;

        for (i = 0; i < vn; i++) {
            aux = longitud(centro.x, centro.y, centro.z,
                vertices[i].x, vertices[i].y, vertices[i].z);
            if (aux > radio)
                radio = aux;//Radio final de nuestra esfera
        }
        this->radius = radio;


        //Dibujo de la esfera
        angulom = 2 * 3.141592654 / meridianos;
        angulop = 3.141592654 / paralelos;

        glDisable(GL_LIGHTING);
        glTranslatef(centro.x, centro.y, centro.z);
        glBegin(GL_LINES);

        for (i = 0; i < meridianos; i++)
        {
            for (j = 0; j < paralelos; j++)
            {
                v1[0] = radio * cos(angulom * i) * sin(angulop * j);
                v1[1] = radio * cos(angulop * j);
                v1[2] = radio * sin(angulom * i) * sin(angulop * j);

                v2[0] = radio * cos(angulom * i) * sin(angulop * (j + 1));
                v2[1] = radio * cos(angulop * (j + 1));
                v2[2] = radio * sin(angulom * i) * sin(angulop * (j + 1));

                v3[0] = radio * cos(angulom * (i + 1)) * sin(angulop * (j + 1));
                v3[1] = radio * cos(angulop * (j + 1));
                v3[2] = radio * sin(angulom * (i + 1)) * sin(angulop * (j + 1));

                v4[0] = radio * cos(angulom * (i + 1)) * sin(angulop * j);
                v4[1] = radio * cos(angulop * j);
                v4[2] = radio * sin(angulom * (i + 1)) * sin(angulop * j);

                glColor3f(1, 0, 0);
                glVertex3fv(v1);
                glVertex3fv(v2);
                glVertex3fv(v3);
                glVertex3fv(v4);
            }
        }
        glEnd();
        glEnable(GL_LIGHTING);

    }else if (colision=='b' || colision=='B') {

        GLfloat minX = 65000, minY = 65000, minZ = 65000;
        GLfloat maxX = -65000, maxY = -65000, maxZ = -65000;

        glDisable(GL_LIGHTING);
        glBegin(GL_LINE_STRIP);

        for (i = 0; i < vn; i++) {
            if (vertices[i].x < minX)
                minX = vertices[i].x;//minimo en x

            if (vertices[i].x > maxX)
                maxX = vertices[i].x;//maximo en x

            if (vertices[i].y < minY)
                minY = vertices[i].y;//minimo en y
            if (vertices[i].y > maxY)
                maxY = vertices[i].y;//maximo en y

            if (vertices[i].z < minZ)
                minZ = vertices[i].z;//minimo en z
            if (vertices[i].z > maxZ)
                maxZ = vertices[i].z;//maximo en z
        }
        this->maxX = maxX; this->maxY = maxY; this->maxZ = maxZ;
        this->minX = minX; this->minY = minY; this->minZ = minZ;

        GLfloat vertice[8][3] = {
                {minX,minY, minZ},
                {minX,minY, maxZ},
                {minX,maxY, maxZ},
                {minX ,maxY, minZ},
                {maxX,minY, minZ},
                {maxX,minY, maxZ},
                {maxX,maxY, maxZ},
                {maxX ,maxY, minZ},
        };


        //Dibujo de la caja
        glVertex3fv(vertice[0]);
        glVertex3fv(vertice[1]);
        glVertex3fv(vertice[2]);
        glVertex3fv(vertice[3]);
        glVertex3fv(vertice[7]);
        glVertex3fv(vertice[4]);
        glVertex3fv(vertice[5]);
        glVertex3fv(vertice[6]);
        glVertex3fv(vertice[2]);
        glVertex3fv(vertice[1]);
        glVertex3fv(vertice[5]);
        glVertex3fv(vertice[4]);
        glVertex3fv(vertice[0]);
        glVertex3fv(vertice[3]);
        glVertex3fv(vertice[7]);
        glVertex3fv(vertice[6]);


        glEnd();
        glEnable(GL_LIGHTING);
        }
        else {
        printf("\nModelo sin colisión\n");
        } 
}

void obj3dmodel::wireDraw()
{
    glBegin(GL_LINES);
    //Dibujamos las caras
    for (int i = 0; i < caras.size(); i++)
    {
        //Almacenamos el valor de los vertices recorriendo cara por cara
        vertex v1 = vertices[caras[i].v1 - 1];
        vertex v2 = vertices[caras[i].v2 - 1];
        vertex v3 = vertices[caras[i].v3 - 1];

        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);

        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
}

//GLfloat *obj3dmodel::getCenter() {
//        return this->center;
//}
//char obj3dmodel::getColision() {
//        return this->colision;
//}

