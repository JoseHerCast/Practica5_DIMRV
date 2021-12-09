
#include <SDL.h>
#include <stdio.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>


#include "texture.h"
#include "figuras.h"
#include "Camera.h"
#include <String.h>
#include "model.h"
#include "terreno.h"

/*NOTA:Por el momento, el cargador de modelos solo cuenta con las siguientes características.

-Carga modelos de manera correcta siempre y cuando esten formados por primitivas triangulos.

-A pesar que puede leer y almacenar las coordenadas de textura, no se implemento su reconocimiento en las
caras ni su carga en memoria y por consiguiente en el modelo.

-Se pueden implementar modelos de luz y material sin problemas ya que si se tomaron en cuenta las normales indicadas
en cada cara

*/

#if (_MSC_VER >= 1900)
#   pragma comment( lib, "legacy_stdio_definitions.lib" )
#endif

//Variables para manejo inicial de SDL

enum class State {PLAY,EXIT};

SDL_DisplayMode dm;
SDL_Window* window = NULL;
SDL_Event evnt;
SDL_GLContext glContext;

State _currentState = State::PLAY;

const Uint8* auxKey = SDL_GetKeyboardState(NULL);

int screenW = 0;
int screenH = 0;

Terrain terreno;
//Escala de nuestro terreno
GLfloat scale[3] = { 100,1,100 };
//Bandera que ayuda a restirngir la camara a dentro del terreno
GLboolean outside = false;
//Pendiente de la recta para determinar la altura de la posición de la camara
GLfloat m;


CCamera objCamera;	//Creamos objeto cámara
CCamera freeCamera;

//Variables para determinar las cordenadas del puntero del raton
int currentX = 0, lastX = 0;
int currentY = 0, lastY = 0;

GLfloat g_lookupdown = 0.0f;    //Variables para rotar cámaras sobre el eje Z (vista arriba/abajo)
GLfloat g_lookupdown2 = 0.0f;

bool vistaAerea = false; //Bandera para cambio de cámara

obj3dmodel corvette;
obj3dmodel goku;
obj3dmodel test;
obj3dmodel test2;
//Modelo de prueba para mostrar como es que se cargan y dibujan los modelos que no estan formados por triangulos
obj3dmodel tie; 
//Variables de posición de modelos
GLfloat posMono[3] = { -10, 10, 40 };
GLfloat posCorvette[3] = { -10, 50, 0 };
GLfloat posGoku[3] = { -60,10,0 };
GLfloat posTest2[3] = { -60,10,40 };
//Variables de escala de modelos
GLfloat scaleMono = 3;
GLfloat scaleCorvette = 3;
GLfloat scaleGoku = 3;
GLfloat scaleTest2 = 3;
//Variables de selección de modelo
bool selNave = false;
bool selGoku = false;
bool selMono = false;
bool selTest2 = false;


//Variables para picking
GLint viewport[4];
GLdouble modelview[16];
GLdouble projection[16];
GLfloat winX, winY, winZ;
POINT mouse;
HWND hWnd;
GLdouble relX, relY, relZ;
GLfloat distGoku,distCorv,distTest,distTest2;
GLfloat pos[3];

CTexture t_sky; //Skybox
CTexture t_sky2; //Skybox

CFiguras sky_cube;

GLfloat l_pos[3] = {1000,500,0};
GLfloat l_ambient[3] = {1.0,1.0,1.0};
GLfloat l_diffuse[3] = { 1.0,1.0,1.0 };
GLfloat l_specular[3] = { 1.0,1.0,1.0 };
//Zafiro
GLfloat m_ambient[3] = { 0.0215,0.1745,0.55};
GLfloat m_diffuse[3]= { 0.7568,0.61424,0.55};
GLfloat m_specular[3] = { 0.633,0.727811,0.55};
GLfloat m_shininess= 76.8;
//Rubi
GLfloat w_ambient[3] = { 0.3745, 0.01175, 0.1 };
GLfloat w_diffuse[3] = { 0.61424,0.04136,0.1};
GLfloat w_specular[3] = { 0.727811,0.226959 ,0.1};
GLfloat w_shininess = 20.8;

GLfloat g_ambient[3] = { 0.01175,0.3745, 0.1 };
GLfloat g_diffuse[3] = { 0.04136,0.61424,0.1 };
GLfloat g_specular[3] = { 0.226959,0.727811,0.1 };
GLfloat g_shininess = 76;

GLfloat ambient[3] = {0.5,0.5,0.5};

//Funciones sobrecargadas de distancia
GLfloat distancia(obj3dmodel model1, obj3dmodel model2,GLfloat *pos1, GLfloat *pos2) {
    return sqrt(pow(((model1.center[0]+pos1[0]) - (model2.center[0]+ pos2[0])), 2) +
                pow(((model1.center[1]+pos1[1]) - (model2.center[1]+ pos2[1])), 2) +
                pow(((model1.center[2]+pos1[2]) - (model2.center[2]+ pos2[2])), 2));
}

GLfloat distancia(obj3dmodel model, GLdouble x, GLdouble y, GLdouble z,GLfloat *pos){
    return sqrt(pow((x - (model.center[0] + pos[0])), 2) +
        pow((y - (model.center[1] + pos[1])), 2) +
        pow((z - (model.center[2] + pos[2])), 2));
}
//Esta función sirve para probar si un punto esta dentro de una caja
bool puntoEnCaja(obj3dmodel model, GLdouble x, GLdouble y, GLdouble z, GLfloat scale,GLfloat *pos) {
    
    if (model.maxX * scale + pos[0] > x && model.maxY * scale + pos[1] > y && model.maxZ * scale + pos[2] > z )
        if(model.minX * scale + pos[0] < x && model.minY * scale + pos[1] < y && model.minZ * scale + pos[2] < z)
        return true;
    else
        return false;
}
//Esta función sirve para probar la colisión de dos cajas
bool cajaEnCaja(obj3dmodel model1, obj3dmodel model2, GLfloat scale1, GLfloat* pos1, GLfloat scale2, GLfloat *pos2) {
    bool x, y, z;

    //De todos los casos posibles entre que haya o no colision de cajas, se decidió evaluar los más sencillos
    //los cuales son en donde no hay colisión, caso contrario si hay colision

    //eje x
    if ((model2.maxX * scale2 + pos2[0] < model1.minX * scale1 + pos1[0]) ||
        (model1.maxX * scale1 + pos1[0] < model2.minX * scale2 + pos2[0]))
        x = false;
    else
        x = true;
    //eje y
    if ((model2.maxY * scale2 + pos2[1] < model1.minY * scale1 + pos1[1]) ||
        (model1.maxY * scale1 + pos1[1] < model2.minY * scale2 + pos2[1]))
        y = false;
    else
        y = true;
    //eje z
    if ((model2.maxZ * scale2 + pos2[2] < model1.minZ * scale1 + pos1[2]) ||
        (model1.maxZ * scale1 + pos1[2] < model2.minZ * scale2 + pos2[2]))
        z = false;
    else
        z = true;

    return x && y && z;
}

bool esferaEnCaja(obj3dmodel model1, obj3dmodel model2, GLfloat scale1, GLfloat* pos1, GLfloat scale2, GLfloat* pos2) {
    bool x, y, z;
    GLfloat d = 0;

    //eje x
    if (model1.center[0] + pos1[0] < model2.minX * scale2 + pos2[0])
        d = d + pow(((model1.center[0]+pos1[0])-(model2.minX*scale2+pos2[0])), 2);
    else if(model1.center[0] + pos1[0] > model2.maxX * scale2 + pos2[0])
        d = d + pow(((model1.center[0] + pos1[0]) - (model2.maxX * scale2 + pos2[0])), 2);
    if (d < model1.radius * scale1)
        x = true;
    else
        x = false;
    //eje y
    if (model1.center[1] + pos1[1] < model2.minY * scale2 + pos2[1])
        d = d + pow(((model1.center[1] + pos1[1]) - (model2.minY * scale2 + pos2[1])), 2);
    else if (model1.center[1] + pos1[1] > model2.maxX * scale2 + pos2[0])
        d = d + pow(((model1.center[1] + pos1[1]) - (model2.maxY * scale2 + pos2[1])), 2);
    if (d < model1.radius * scale1)
        y = true;
    else
        y = false;
    //eje z
    if (model1.center[2] + pos1[2] < model2.minZ * scale2 + pos2[2])
        d = d + pow(((model1.center[2] + pos1[2]) - (model2.minZ * scale2 + pos2[2])), 2);
    else if (model1.center[2] + pos1[2] > model2.maxZ * scale2 + pos2[2])
        d = d + pow(((model1.center[2] + pos1[2]) - (model2.maxZ * scale2 + pos2[2])), 2);
    if (d < model1.radius * scale1)
        z = true;
    else
        z = false;

    return x && y && z;
}


void Texture_Load() {

    t_sky.LoadTGA("Textures/Sky/sky5.tga");
    t_sky.BuildGLTexture();
    t_sky.ReleaseImage();

    t_sky2.LoadTGA("Textures/Sky/sky6.tga");
    t_sky2.BuildGLTexture();
    t_sky2.ReleaseImage();
}

void cargaModelos() {
    corvette.readfile("Modelos/Corvette/corvette.obj");
    goku.readfile("Modelos/Goku/goku.obj");
    test.readfile("Modelos/test.obj");
    test2.readfile("Modelos/test.obj");
    tie.readfile("Modelos/Tie-Fighter.obj");

}

void fatalError(const char* error) {
    printf(error, "\n\nPresione cualquier tecla par salir...");
    char tmp;
    scanf("%c",&tmp);
    SDL_Quit();
}

void initSDL() {
    
    //Inicializamos SDL con todos sus subsistemas
    SDL_Init(SDL_INIT_EVERYTHING);

	//Version OpenGL 2.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    //Buffer de dibujo doble para poder realizar el repintado de manera mas suave
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    //Muestreo multiple para efecto alising
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    //Creamos nuestra ventana
    window = SDL_CreateWindow("Primer ejemplo en SDL",
        SDL_WINDOWPOS_UNDEFINED,    //Posición X
        SDL_WINDOWPOS_UNDEFINED,    //Posición Y
        600,                        //Width
        400,                        //Height
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);//Usaremos OpenGL para dibujar

    
    if (window == nullptr) {
        fatalError("\nSDL no pudo crear la ventana\n");
    }
	//Creamos el contexto de renderizado para OpenGL
    SDL_GLContext glContext=SDL_GL_CreateContext(window);
    
    if (glContext == nullptr) {
        fatalError("\nSDL no pudo crear el contexto para OpenGL\n");
    }

	/* This makes our buffer swap syncronized with the monitor's vertical refresh */
	SDL_GL_SetSwapInterval(1);

  /*  GLenum error = glewInit();
    if (error != GLEW_OK) {
        fatalError("\nNo se pudo iniciar GLEW\n");
    }*/

}

void initGL(void)     // Inicializamos parametros
{
    //Inicializamos la matriz de proyección (perspectiva)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glFrustum(-0.1, 0.1, -0.1, 0.1, 0.1, 20000.0);
	//Inicializamos la matriz de vista-modelo (renderizado)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


    glEnable(GL_LIGHTING);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0,GL_POSITION,l_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, l_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, l_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, l_specular);


	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Negro de fondo
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);									// Configuramos Buffer de profundidad
    glEnable(GL_DEPTH_TEST);							// Habilitamos Depth Testing
    glDepthFunc(GL_LEQUAL);								// Tipo de Depth Testing a realizar
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);

    Texture_Load();

    objCamera.Position_Camera(-6, 2.5f, 3, 10, 2.5f, 0, 0, 1, 0);
    freeCamera.Position_Camera(-6, 2.5f, 3, 10, 2.5f, 0, 0, 1, 0);

    GLint redBits, greenBits, blueBits; 
    glGetIntegerv(GL_RED_BITS, &redBits); 
    glGetIntegerv(GL_GREEN_BITS, &greenBits); 
    glGetIntegerv(GL_BLUE_BITS, &blueBits);

    printf("Color depth (%d,%d,%d)",redBits,greenBits,blueBits);
}

void sdlDisplay() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Limpiamos pantalla y profundidad
	glLoadIdentity();
    
    CFiguras ejemplo;


    if (vistaAerea == true) {
        glRotatef(g_lookupdown2, 1.0f, 0, 0);

        gluLookAt(freeCamera.mPos.x, freeCamera.mPos.y, freeCamera.mPos.z,
            freeCamera.mView.x, freeCamera.mView.y, freeCamera.mView.z,
            freeCamera.mUp.x, freeCamera.mUp.y, freeCamera.mUp.z);
    }
    else {
        glRotatef(g_lookupdown, 1.0f, 0, 0);

        gluLookAt(objCamera.mPos.x, objCamera.mPos.y, objCamera.mPos.z,
            objCamera.mView.x, objCamera.mView.y, objCamera.mView.z,
            objCamera.mUp.x, objCamera.mUp.y, objCamera.mUp.z);
    }

    glPushMatrix(); //Se pinta el cielo
    glDisable(GL_LIGHTING);
        sky_cube.skybox(10000.0, 5000.0, 5000.0, t_sky.GLindex);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(posCorvette[0],posCorvette[1],posCorvette[2]);
    glScalef(scaleCorvette, scaleCorvette, scaleCorvette);
    glColor3f(0.5, 0.5, 0.5);
    corvette.solidDraw(m_ambient,m_diffuse,m_specular,m_shininess,'s');
    //corvette.wireDraw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(posGoku[0],posGoku[1],posGoku[2]);
    glScalef(scaleGoku, scaleGoku, scaleGoku);
    glColor3f(0.5, 0.5, 0.5);
    goku.solidDraw(m_ambient, m_diffuse, m_specular, m_shininess,'b');
    //goku.wireDraw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(posMono[0], posMono[1], posMono[2]);
    glScalef(scaleMono, scaleMono, scaleMono);
    glColor3f(0.5, 0.5, 0.5);
    test.solidDraw(m_ambient, m_diffuse, m_specular, m_shininess,'s');
    //test.wireDraw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(posTest2[0], posTest2[1], posTest2[2]);
    glScalef(scaleTest2, scaleTest2, scaleTest2);
    glColor3f(0.5, 0.5, 0.5);
    test2.solidDraw(m_ambient, m_diffuse, m_specular, m_shininess, 'b');
    //test.wireDraw();
    glPopMatrix();

    //glPushMatrix();
    //glTranslatef(-40, 0, -40);
    //glScalef(3.0, 3.0, 3.0);
    //glColor3f(0.5, 0.5, 0.5);
    //tie.solidDraw(w_ambient, w_diffuse, w_specular, w_shininess);
    ////tie.wireDraw();
    //glPopMatrix();

    //Coordenadas
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);//Color rojo
    glBegin(GL_LINES);//Eje X
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(100.0, 0.0, 0.0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glColor3f(0.0, 1.0, 0.0);//Color verde
    glBegin(GL_LINES);//Eje Y
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 100.0, 0.0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glColor3f(0.0, 0.0, 1.0);//Color azul
    glBegin(GL_LINES);//Eje Z
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 100.0);
    glEnd();
    glPopMatrix();
    glEnable(GL_LIGHTING);

    //Terreno
    glPushMatrix();
    glScalef(scale[0], scale[1], scale[2]);
    terreno.solidDraw(g_ambient, g_diffuse, g_specular, g_shininess);
    //terreno.wireDraw();
    glPopMatrix();

    //Prueba de picking
    glPushMatrix();
    glColor3f(1, 1, 1);
    glTranslatef(relX, relY, relZ);
    ejemplo.esfera(0.1, 15, 15, NULL);
    glPopMatrix();

    SDL_GL_SwapWindow(window);
}

void reshape(int width, int height)   // Creamos funcion Reshape
{
    if (height == 0)										// Prevenir division entre cero
    {
        height = 1;
    }

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);						// Seleccionamos Projection Matrix
    glLoadIdentity();

    // Tipo de Vista

    glFrustum(-0.1, 0.1, -0.1, 0.1, 0.1, 20000.0);

    glMatrixMode(GL_MODELVIEW);							// Seleccionamos Modelview Matrix
    glLoadIdentity();
}

void picking_pipeline() {

    //Con esto solo se obtendrá un vector de dirección con coordenadas relativas al viewport

    glGetIntegerv(GL_VIEWPORT, viewport);//{x,y,width,height} Retrieves The Viewport Values (X, Y, Width, Height)
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);// Retrieve The Modelview Matrix
    glGetDoublev(GL_PROJECTION_MATRIX, projection);// Retrieve The Projection Matrix

    mouse.x = lastX;    // Gets The Current Cursor Coordinates (Mouse Coordinates)
    mouse.y = lastY;
    ScreenToClient(hWnd, &mouse);

    winX = (float)mouse.x;// Holds The Mouse X Coordinate
    winY = (float)mouse.y;// Holds The Mouse Y Coordinate

    winY = (float)viewport[3] - winY;// Subtract The Current Mouse Y Coordinate From The Screen Height.

    glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &relX, &relY, &relZ);

    //////////////////////////////////////////////////////////////////////////////

    //Ahora hay que calcular la posición en el mundo real del objeto
}

void processInput() {

    SDL_GetMouseState(&lastX, &lastY);
    SDL_GetWindowSize(window, &screenW, &screenH);
    //Manejo de interacción con el usuario.
    while (SDL_PollEvent(&evnt) == 1) {
        switch (evnt.type) {
        case SDL_QUIT:
            _currentState = State::EXIT;
            break;
        //Cuando se redimensione la ventana
        case SDL_WINDOWEVENT:
            if (evnt.window.event == SDL_WINDOWEVENT_RESIZED) {
                reshape(evnt.window.data1, evnt.window.data2);
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
          
            if (evnt.button.button == SDL_BUTTON_LEFT) {
                picking_pipeline();             

                //*********************Modelo de colisión esfera*********************************//

                //¿El punto del picking esta dentro de la esfera de alguno de los modelos de colisión?
                //Calculamos distancias

                if (corvette.colision == 's' || corvette.colision == 'S')//Si el modelo de colision de la nave es esfera
                    distCorv = distancia(corvette,relX,relY,relZ,posCorvette);
                if (goku.colision == 's' || goku.colision == 'S')//Si el modelo de colision de goku es esfera
                    distGoku = distancia(goku,relX,relY,relZ,posGoku);
                if (test.colision == 's' || test.colision == 'S')//Si el modelo de colisión del mono es esfera
                    distTest = distancia(test,relX,relY,relZ,posMono);
                if (test.colision == 's' || test.colision == 'S')//Si el modelo de colisión del mono es esfera
                    distTest2 = distancia(test2, relX, relY, relZ, posTest2);
                
                if (distCorv < (corvette.radius * scaleCorvette)) 
                {
                        printf("\n\n+++++Nave seleccionada++++++\n\n");
                        selNave = true;
                        selGoku = false;
                        selMono = false;
                        selTest2 = false;

                }else if (distGoku < (goku.radius * scaleGoku)) 
                {
                        printf("\n\n+++++Goku seleccionado++++++\n\n");
                        selNave = false;
                        selGoku = true;
                        selMono = false;
                        selTest2 = false;

                }else if (distTest < (test.radius * scaleMono)) 
                {
                        printf("\n\n+++++Mono seleccionado++++++\n\n");
                        selNave = false;
                        selGoku = false;
                        selMono = true;
                        selTest2 = false;

                } else if(distTest2<(test2.radius*scaleTest2)){
                    printf("\n\n+++++Mono2 seleccionado++++++\n\n");
                    selNave = false;
                    selGoku = false;
                    selMono = false;
                    selTest2 = true;
                }else {//ningun modelo seleccionado
                    selNave = false;
                    selGoku = false;
                    selMono = false;
                }

                //*********************Modelo de colisión caja*********************************//

                if (corvette.colision == 'b' || corvette.colision == 'B')//Si el modelo es una caja
                    if (puntoEnCaja(corvette,relX,relY,relZ,scaleCorvette,posCorvette)) {
                        printf("\n\n+++++Nave seleccionado++++++\n\n");
                        selNave = true;
                        selGoku = false;
                        selMono = false;
                        selTest2 = false;
                    }
                if (goku.colision == 'b' || goku.colision == 'B')//Si el modelo es una caja
                    if (puntoEnCaja(goku,relX,relY,relZ,scaleGoku,posGoku)){
                        printf("\n\n+++++Goku seleccionado++++++\n\n");
                        selNave = false;
                        selGoku = true;
                        selMono = false;
                        selTest2 = false;
                    }
                if (test.colision == 'b' || test.colision == 'B')//Si el modelo es una caja
                    if (puntoEnCaja(test,relX,relY,relZ,scaleMono,posMono)) {
                        printf("\n\n+++++Mono seleccionado++++++\n\n");
                        selNave = false;
                        selGoku = false;
                        selMono = true;
                        selTest2 = false;
                    }
                if (test2.colision == 'b' || test2.colision == 'B')//Si el modelo es una caja
                    if (puntoEnCaja(test2, relX, relY, relZ, scaleTest2, posTest2)) {
                        printf("\n\n+++++Mono2 seleccionado++++++\n\n");
                        selNave = false;
                        selGoku = false;
                        selMono = false;
                        selTest2 = true;
                    }
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (evnt.button.button == SDL_BUTTON_LEFT) {

                relX = relY = relZ = 0;
            }
            break;
        case SDL_MOUSEMOTION:
         
            if (evnt.button.button == SDL_BUTTON(3)) {

                currentX = evnt.motion.x;
                currentY = evnt.motion.y;

                if (vistaAerea == true) {
                    if (currentX < (lastX))
                        freeCamera.Rotate_View(-CAMERASPEED);
                    else if (currentX > (lastX))
                        freeCamera.Rotate_View(CAMERASPEED);

                    if (currentY < (lastY))
                        g_lookupdown2 -= 1.3;
                    else if (currentY > (lastY))
                        g_lookupdown2 += 1.3;
                }
                else {
                    if (currentX < (lastX))
                        objCamera.Rotate_View(-CAMERASPEED);
                    else if (currentX > (lastX))
                        objCamera.Rotate_View(CAMERASPEED);

                    if (currentY < (lastY))
                        g_lookupdown -= 1.3;
                    else if (currentY > (lastY))
                        g_lookupdown += 1.3;
                }
                //printf("Mouse coordinates: (%d,%d)\nWindow size: %d W %d H\n\n",
                //    currentX,
                //    currentY,
                //    screenW,
                //    screenH);

            }
            break;
        //Todo lo relacionado con presionar alguna tecla
        case SDL_KEYDOWN:

            switch (evnt.key.keysym.sym) {
                //Manejo de teclas alfabéticas
            case 'a':
            case 'A':

                if (vistaAerea == true) {
                    freeCamera.Strafe_Camera(-(CAMERASPEED + 0.1));
                }
                else {
                    if (objCamera.mPos.x > scale[0] || objCamera.mPos.x<(-scale[0]) ||
                        objCamera.mPos.z>scale[2] || objCamera.mPos.z < (-scale[2])) {
                        printf("\nCoordenadas de cámara: %f,%f", objCamera.mPos.x, objCamera.mPos.z);
                        printf("\nMáximos XZ: ( %f,%f )\n", scale[0], scale[2]);
                        printf("\nMinimos XZ: ( %f,%f )\n", -scale[0], -scale[2]);
                        outside = true;
                    }
                    else {
                        outside = false;
                        objCamera.Strafe_Camera(-(CAMERASPEED + 0.1));
                        //Si estamos sobre el plano inclinado
                        if (objCamera.mPos.x >= 0) {
                            //Calculamos el valor de y acorde a la ecuación de la recta y la pendiente obtenida
                            objCamera.mPos.y = objCamera.mPos.x * m;
                            objCamera.mView.y = (objCamera.mPos.x + 10) * m;
                        }
                        else {
                            objCamera.mPos.y = 2.5;
                            objCamera.mView.y = (objCamera.mPos.x + 10) * (2.5 / scale[0]);
                        }
                    }

                    if (outside == true) {
                        if (objCamera.mPos.x > scale[0] && objCamera.mPos.z<scale[2] && objCamera.mPos.z >(-scale[2])) {
                            objCamera.mPos.x -= 1;
                            objCamera.mView.x -= 1;
                        }
                        else if (objCamera.mPos.x < (-scale[0]) && objCamera.mPos.z<scale[2] && objCamera.mPos.z >(-scale[2])) {
                            objCamera.mPos.x += 1;
                            objCamera.mView.x += 1;

                        }
                        else if (objCamera.mPos.z > scale[2] && objCamera.mPos.x > -scale[0] && objCamera.mPos.x < scale[0]) {
                            objCamera.mPos.z -= 1;
                            objCamera.mView.z -= 1;

                        }
                        else if (objCamera.mPos.z < (-scale[2]) && objCamera.mPos.x > -scale[0] && objCamera.mPos.x < scale[0]) {
                            objCamera.mPos.z += 1;
                            objCamera.mView.z += 1;

                        }
                    }
                }
                break;
            case 'd':
            case 'D':
                if (vistaAerea == true) {
                    freeCamera.Strafe_Camera(CAMERASPEED + 0.1);
                }
                else {
                    if (objCamera.mPos.x > scale[0] || objCamera.mPos.x<(-scale[0]) ||
                        objCamera.mPos.z>scale[2] || objCamera.mPos.z < (-scale[2])) {
                        printf("\nCoordenadas de cámara: %f,%f", objCamera.mPos.x, objCamera.mPos.z);
                        printf("\nMáximos XZ: ( %f,%f )\n", scale[0], scale[2]);
                        printf("\nMinimos XZ: ( %f,%f )\n", -scale[0], -scale[2]);
                        outside = true;
                    }
                    else {
                        outside = false;
                        objCamera.Strafe_Camera(CAMERASPEED + 0.1);
                        //Si estamos sobre el plano inclinado
                        if (objCamera.mPos.x >= 0) {
                            //Calculamos el valor de y acorde a la ecuación de la recta y la pendiente obtenida
                            objCamera.mPos.y = objCamera.mPos.x * m;
                            objCamera.mView.y = (objCamera.mPos.x + 10) * m;
                        }
                        else {
                            objCamera.mPos.y = 2.5;
                            objCamera.mView.y = (objCamera.mPos.x + 10) * (2.5 / scale[0]);
                        }
                    }
                    if (outside == true) {
                        if (objCamera.mPos.x > scale[0] && objCamera.mPos.z<scale[2] && objCamera.mPos.z >(-scale[2])) {
                            objCamera.mPos.x -= 1;
                            objCamera.mView.x -= 1;
                        }
                        else if (objCamera.mPos.x < (-scale[0]) && objCamera.mPos.z<scale[2] && objCamera.mPos.z >(-scale[2])) {
                            objCamera.mPos.x += 1;
                            objCamera.mView.x += 1;

                        }
                        else if (objCamera.mPos.z > scale[2] && objCamera.mPos.x > -scale[0] && objCamera.mPos.x < scale[0]) {
                            objCamera.mPos.z -= 1;
                            objCamera.mView.z -= 1;

                        }
                        else if (objCamera.mPos.z < (-scale[2]) && objCamera.mPos.x > -scale[0] && objCamera.mPos.x < scale[0]) {
                            objCamera.mPos.z += 1;
                            objCamera.mView.z += 1;

                        }
                    }
                }
                break;
            case 'w':
            case 'W':
                if (vistaAerea == true) {
                    if (auxKey[SDL_SCANCODE_RSHIFT] || auxKey[SDL_SCANCODE_LSHIFT])
                        freeCamera.UpDown_Camera(CAMERASPEED + 0.1);
                    else
                        freeCamera.Move_Camera(CAMERASPEED + 0.1);
                }
                else {
                    if (objCamera.mPos.x > scale[0] || objCamera.mPos.x<(-scale[0]) ||
                        objCamera.mPos.z>scale[2] || objCamera.mPos.z < (-scale[2])) {
                        printf("\nCoordenadas de cámara: %f,%f", objCamera.mPos.x, objCamera.mPos.z);
                        printf("\nMáximos XZ: ( %f,%f )\n", scale[0], scale[2]);
                        printf("\nMinimos XZ: ( %f,%f )\n", -scale[0], -scale[2]);
                        outside = true;
                    }
                    else {
                        outside = false;
                        objCamera.Move_Camera(CAMERASPEED + 0.1);
                        //Si estamos sobre el plano inclinado
                        if (objCamera.mPos.x >= 0) {
                            //Calculamos el valor de y acorde a la ecuación de la recta y la pendiente obtenida
                            objCamera.mPos.y = objCamera.mPos.x * m;
                            objCamera.mView.y = (objCamera.mPos.x + 10) * m;
                        }
                        else {
                            objCamera.mPos.y = 2.5;
                            objCamera.mView.y = (objCamera.mPos.x + 10) * (2.5 / scale[0]);
                        }
                    }
                    if (outside == true) {
                        if (objCamera.mPos.x > scale[0] && objCamera.mPos.z<scale[2] && objCamera.mPos.z >(-scale[2])) {
                            objCamera.mPos.x -= 1;
                            objCamera.mView.x -= 1;
                        }
                        else if (objCamera.mPos.x < (-scale[0]) && objCamera.mPos.z<scale[2] && objCamera.mPos.z >(-scale[2])) {
                            objCamera.mPos.x += 1;
                            objCamera.mView.x += 1;

                        }
                        else if (objCamera.mPos.z > scale[2] && objCamera.mPos.x > -scale[0] && objCamera.mPos.x < scale[0]) {
                            objCamera.mPos.z -= 1;
                            objCamera.mView.z -= 1;

                        }
                        else if (objCamera.mPos.z < (-scale[2]) && objCamera.mPos.x > -scale[0] && objCamera.mPos.x < scale[0]) {
                            objCamera.mPos.z += 1;
                            objCamera.mView.z += 1;

                        }
                    }
                }
                break;
            case 's':
            case 'S':
                if (vistaAerea == true) {
                    if (auxKey[SDL_SCANCODE_RSHIFT] || auxKey[SDL_SCANCODE_LSHIFT])
                        freeCamera.UpDown_Camera(-(CAMERASPEED + 0.1));
                    else
                        freeCamera.Move_Camera(-(CAMERASPEED + 0.1));
                }
                else {
                    if (objCamera.mPos.x > scale[0] || objCamera.mPos.x<(-scale[0]) ||
                        objCamera.mPos.z>scale[2] || objCamera.mPos.z < (-scale[2])) {
                        printf("\nCoordenadas de cámara: %f,%f", objCamera.mPos.x, objCamera.mPos.z);
                        printf("\nMáximos XZ: ( %f,%f )\n", scale[0], scale[2]);
                        printf("\nMinimos XZ: ( %f,%f )\n", -scale[0], -scale[2]);
                        outside = true;
                    }
                    else {
                        outside = false;
                        //if (auxKey[SDL_SCANCODE_RSHIFT] || auxKey[SDL_SCANCODE_LSHIFT])
                        //    objCamera.UpDown_Camera(-(CAMERASPEED + 0.1));
                        //else
                        objCamera.Move_Camera(-(CAMERASPEED + 0.1));
                        //Si estamos sobre el plano inclinado
                        if (objCamera.mPos.x >= 0) {
                            //Calculamos el valor de y acorde a la ecuación de la recta y la pendiente obtenida
                            objCamera.mPos.y = objCamera.mPos.x * m;
                            objCamera.mView.y = (objCamera.mPos.x + 10) * m;
                        }
                        else {
                            objCamera.mPos.y = 2.5;
                            objCamera.mView.y = (objCamera.mPos.x + 10) * (2.5 / scale[0]);
                        }
                    }

                    if (outside == true) {
                        if (objCamera.mPos.x > scale[0] && objCamera.mPos.z<scale[2] && objCamera.mPos.z >(-scale[2])) {
                            objCamera.mPos.x -= 1;
                            objCamera.mView.x -= 1;
                        }
                        else if (objCamera.mPos.x < (-scale[0]) && objCamera.mPos.z<scale[2] && objCamera.mPos.z >(-scale[2])) {
                            objCamera.mPos.x += 1;
                            objCamera.mView.x += 1;

                        }
                        else if (objCamera.mPos.z > scale[2] && objCamera.mPos.x > -scale[0] && objCamera.mPos.x < scale[0]) {
                            objCamera.mPos.z -= 1;
                            objCamera.mView.z -= 1;

                        }
                        else if (objCamera.mPos.z < (-scale[2]) && objCamera.mPos.x > -scale[0] && objCamera.mPos.x < scale[0]) {
                            objCamera.mPos.z += 1;
                            objCamera.mView.z += 1;

                        }
                    }
                }
                break;
                //Manejo de teclas especiales como su valor en ASCII (opcional, SDL ya cuenta con constantes)
            case 'f':
            case 'F':
                vistaAerea ^= true;
                break;
            case SDLK_UP:
                if (auxKey[SDL_SCANCODE_LSHIFT] || auxKey[SDL_SCANCODE_RSHIFT]) {
                    if (selGoku) {
                        posGoku[1] += 0.5;
                    }
                    if (selMono) {
                        posMono[1] += 0.5;
                    }
                    if (selNave) {
                        posCorvette[1] += 0.5;
                    }
                    if (selTest2) {
                        posTest2[1] += 0.5;
                    }
                }
                else {
                    if (selGoku) {
                        posGoku[2] += 0.5;
                    }
                    if (selMono) {
                        posMono[2] += 0.5;
                    }
                    if (selNave) {
                        posCorvette[2] += 0.5;
                    }
                    if (selTest2) {
                        posTest2[2] += 0.5;
                    }
                }
                //Ejemplo de colision esfera-esfera
                if (distancia(test, corvette, posMono, posCorvette)
                    < (test.radius * scaleMono + corvette.radius * scaleCorvette))
                    printf("\nColision entre nave-mono\n");
                //Ejemplo de colision caja-caja
                if (cajaEnCaja(goku,test2,scaleGoku,posGoku,scaleTest2,posTest2)) {
                    printf("\nColision entre goku-mono2\n");
                }
                //Ejemplo de colision esfera-caja
                if (cajaEnCaja(test, test2, scaleMono, posMono, scaleTest2, posTest2)) {
                    printf("\nColision entre mono1-mono2\n");
                }
                break;
            case SDLK_DOWN:
                if (auxKey[SDL_SCANCODE_LSHIFT] || auxKey[SDL_SCANCODE_RSHIFT]) {
                    if (selGoku) {
                        posGoku[1] -= 0.5;
                    }
                    if (selMono) {
                        posMono[1] -= 0.5;
                    }
                    if (selNave) {
                        posCorvette[1] -= 0.5;
                    }
                    if (selTest2) {
                        posTest2[1] -= 0.5;
                    }
                }
                else {
                    if (selGoku) {
                        posGoku[2] -= 0.5;
                    }
                    if (selMono) {
                        posMono[2] -= 0.5;
                    }
                    if (selNave) {
                        posCorvette[2] -= 0.5;
                    }
                    if (selTest2) {
                        posTest2[2] -= 0.5;
                    }
                }
                //Ejemplo de colision esfera-esfera
                if (distancia(test, corvette, posMono, posCorvette)
                    < (test.radius * scaleMono + corvette.radius * scaleCorvette))
                    printf("\nColision entre nave-mono\n");
                //Ejemplo de colision caja-caja
                if (cajaEnCaja(goku, test2, scaleGoku, posGoku, scaleTest2, posTest2)) {
                    printf("\nColision entre goku-mono2\n");
                }
                //Ejemplo de colision esfera-caja
                if (cajaEnCaja(test, test2, scaleMono, posMono, scaleTest2, posTest2)) {
                    printf("\nColision entre mono1-mono2\n");
                }
                break;
            case SDLK_RIGHT:
                if (selGoku) {
                    posGoku[0] += 0.5;
                }
                if (selMono) {
                    posMono[0] += 0.5;
                }
                if (selNave) {
                    posCorvette[0] += 0.5;
                }
                if (selTest2) {
                    posTest2[0] += 0.5;
                }
                //Ejemplo de colision esfera-esfera
                if (distancia(test, corvette, posMono, posCorvette)
                    < (test.radius * scaleMono + corvette.radius*scaleCorvette))
                    printf("\nColision entre nave-mono\n");
                //Ejemplo de colision caja-caja
                if (cajaEnCaja(goku, test2, scaleGoku, posGoku, scaleTest2, posTest2)) {
                    printf("\nColision entre goku-mono2\n");
                }
                //Ejemplo de colision esfera-caja
                if (cajaEnCaja(test, test2, scaleMono, posMono, scaleTest2, posTest2)) {
                    printf("\nColision entre mono1-mono2\n");
                }
                break;
            case SDLK_LEFT:
                if (selGoku) {
                    posGoku[0] -= 0.5;
                }
                if (selMono) {
                    posMono[0] -= 0.5;
                }
                if (selNave) {
                    posCorvette[0] -= 0.5;
                }
                if (selTest2) {
                    posTest2[0] -= 0.5;
                }
                //Ejemplo de colision esfera-esfera
                if (distancia(test, corvette, posMono, posCorvette)
                    < (test.radius * scaleMono + corvette.radius * scaleCorvette))
                    printf("\nColision entre nave-mono\n");
                //Ejemplo de colision caja-caja
                if (cajaEnCaja(goku, test2, scaleGoku, posGoku, scaleTest2, posTest2)) {
                    printf("\nColision entre goku-mono2\n");
                }
                //Ejemplo de colision esfera-caja
                if (cajaEnCaja(test, test2, scaleMono, posMono, scaleTest2, posTest2)) {
                    printf("\nColision entre mono1-mono2\n");
                }
                break;
            case 27:        // Cuando Esc es presionado...
                exit(0);   // Salimos del programa
                break;
            default:        // Cualquier otra
                break;

            }
            break;
        default:
                break;
        }
    }
}

void sdlMainLoop() {
    
    while (_currentState != State::EXIT) {
        processInput();
        sdlDisplay();
    }
}

int main(int argc, char **argv)
{ 
    initSDL();

    SDL_GetDesktopDisplayMode(0,&dm);
    if (window == nullptr) {
        fatalError("\nSDL no pudo obtener el modo de pantalla\n");
    }


    initGL();

    cargaModelos();
    terreno.generateMesh(100,100);
    //El plano inclinado esta sobre el eje X, calculamos el valor de la pendiente del plano.
    m=terreno.getHeight()/scale[0];
    printf("\nPendiente igual a: %lf",m);

    sdlMainLoop();

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();

    return 0;
}



