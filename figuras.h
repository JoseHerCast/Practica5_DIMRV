#include <windows.h>		// Archivo de cabecera para Windows
#include <math.h>			// Archivo de cabecera para Funciones Matemáticas
#include <stdio.h>			// Header File For Standard Input/Output
#include <stdlib.h>			// Header File For Standard Library
#include <SDL_opengl.h>


class CFiguras
{
	public:

	float text_der;
	float text_izq;

	void esfera(GLfloat radio, int meridianos, int paralelos, GLuint text); //Funcíon creacion esfera
	void cilindro(float radio, float altura, int resolucion, GLuint text);	//Funcíon creacion cilindro
	void cono(float altura, float radio, int resolucion, GLuint text);		//Funcíon creacion cono
	void cubemap(GLuint text, GLuint text2, GLuint text3, GLuint text4);
	void prisma (float altura, float largo, float profundidad, GLuint text);//Funcíon creacion prisma
	void prisma2(GLuint text, GLuint text2);
	void prisma_r_u(GLuint text, GLuint text2);
	void prisma_l_o(GLuint text, GLuint text2);
	void prisma3(GLuint text, GLuint text2);
	void plano(GLfloat *normal);
	void skybox(float altura, float largo, float profundidad, GLuint text);
	void torus(GLfloat radioM, GLfloat radiom, int meridianos, int paralelos);
	void arco(GLfloat radioM, GLfloat radiom, int meridianos, int paralelos);
	void medio_arco(GLfloat radioM, GLfloat radiom, int meridianos, int paralelos);
	void cup(GLfloat radio, int meridianos, int paralelos, GLuint text);
	void texturaGUI(GLuint text);

};
