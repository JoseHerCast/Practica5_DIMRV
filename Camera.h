#include <windows.h>		// Archivo de cabecera para Windows
#include <math.h>			// Archivo de cabecera para Funciones Matem�ticas
#include <stdio.h>			// Header File For Standard Input/Output
#include <stdlib.h>			// Header File For Standard Library
#include <SDL_opengl.h>

#define CAMERASPEED	0.05f //Constante de velocidad de la c�mara

typedef struct tVector3
{			
	tVector3() {}	// constructor
	tVector3 (float new_x, float new_y, float new_z) //Inicializamos el constructor de nuestra estructura tVector3	 
	{x = new_x; y = new_y; z = new_z;}
	// Sobre cargamos el operador + para sumar dos vectores de manera sencilla
	tVector3 operator+(tVector3 vVector) {return tVector3(vVector.x+x, vVector.y+y, vVector.z+z);}
	// Sobrecargamos el operador - de igual forma para restar dos vectores de manera sencilla
	tVector3 operator-(tVector3 vVector) {return tVector3(x-vVector.x, y-vVector.y, z-vVector.z);}
	// Sobrecargamos el operador * para multiplicar vectores por un escalar
	tVector3 operator*(float number)	 {return tVector3(x*number, y*number, z*number);}
	// Sobrecargamos el operador / para dividir vectores por un escalar
	tVector3 operator/(float number)	 {return tVector3(x/number, y/number, z/number);}
	
	float x, y, z;						// Estas son las coordenadas en 3d de nuestro vector
}tVector3;

class CCamera 
{
	public:
		//Los tres vectores b�sicos de toda c�mara (posici�n, vista y UP)
		tVector3 mPos;	
		tVector3 mView;		
		tVector3 mUp;
		//Funci�n para realizar un movimiento lateral de nuestra c�mara.
		void Strafe_Camera(float speed);
		//Funci�n para avanzar o retroceder la c�mara.
		void Move_Camera(float speed);
		//Funci�n para mover la vista de la c�mara
		void Rotate_View(float speed);
		//Funci�n para subir o bajar sobre el eje Y
		void UpDown_Camera(float speed);
		//Inicializamos el objeto c�mara con los valores de posici�n, vista y UP que hemos pasado como parametros
		void Position_Camera(float pos_x,	float pos_y,	float pos_z,
			 				 float view_x,	float view_y,	float view_z,
							 float up_x,	float up_y,		float up_z);
};
