
#include "Camera.h"


//En primer lugar, tenemos que inicializar los valores de los tres vectores de la cámara, estos son pasados
//como parámetros de la función y es importante tomar en cuenta que son vectores relativos al origen
void CCamera::Position_Camera(float pos_x,  float pos_y,  float pos_z,
							  float view_x, float view_y, float view_z,
							  float up_x,   float up_y,   float up_z)
{
	mPos	= tVector3(pos_x,  pos_y,  pos_z ); // inicializamos vector de posición
	mView	= tVector3(view_x, view_y, view_z); // inicializamos vector de vista
	mUp		= tVector3(up_x,   up_y,   up_z  ); // inicializamos vector UP
}

//Movimiento Avanzar/Retroceder
void CCamera::Move_Camera(float speed)
{
	//En cualquier desplazamiento de la posición de la cámara, es necesario tambien modificar el vector vista.
	tVector3 vVector = mView - mPos;	//Obtenemos el vector vista pero ahora relativo a la posición de la cámara
	
	//Para avanzar, se utilizan valores positivos del parámetro speed 
	//Para retroceder, valores negativos (CAMERASPEED).
	//Como se comento anteriormente, se modifican tanto la posición como la vista en la misma medida.
	//El movimiento se realiza sobre el plano XZ
	mPos.x  = mPos.x  + vVector.x * speed;
	mPos.z  = mPos.z  + vVector.z * speed;
	mView.x = mView.x + vVector.x * speed;
	mView.z = mView.z + vVector.z * speed;
}

//Movimiento de la vista de cámara
void CCamera::Rotate_View(float speed)
{
	//Calculamos el vector vista respecto a la posición de la cámara
	tVector3 vVector = mView - mPos;
	//Dado que el movimiento de la vista que buscamos es el de rotar sobre un punto (posición de cámara),
	//los puntos finales del vector de vista estarán circunscritos, por lo que debemos obtener la proyección
	//en cada uno de los ejes (Seno y Coseno) tanto para la nueva componente en z como en x
	mView.z = (float)(mPos.z + sin(speed)*vVector.x + cos(speed)*vVector.z);
	mView.x = (float)(mPos.x + cos(speed)*vVector.x - sin(speed)*vVector.z);
}

//Movimiento lateral
void CCamera::Strafe_Camera(float speed)
{	
	//Calculamos el vector vista respecto a la posición de la cámara
	tVector3 vVector = mView - mPos;
	tVector3 vOrthoVector;//Vector ortogonal al vector vista

	vOrthoVector.x = -vVector.z;
	vOrthoVector.z =  vVector.x;

	//Para la izquierda, se utilizan valores positivos del parámetro speed 
	//Para la derecha, valores negativos (CAMERASPEED).
	//Como se comento anteriormente, se modifican tanto la posición como la vista en la misma medida.
	//El movimiento se realiza sobre el plano XZ
	mPos.x  = mPos.x  + vOrthoVector.x * speed;
	mPos.z  = mPos.z  + vOrthoVector.z * speed;
	mView.x = mView.x + vOrthoVector.x * speed;
	mView.z = mView.z + vOrthoVector.z * speed;
}


void CCamera::UpDown_Camera(float speed)
{
	//Calculamos el vector vista respecto a la posición de la cámara
	tVector3 vVector = mView - mPos;	
	tVector3 vOrthoVector;//Vector ortogonal al vector vista

	vOrthoVector.y = 10;

	//Arriba valores positivos de speed y abajo valores negativos de speed (CAMERASPEED).
	mPos.y  = mPos.y  + vOrthoVector.y * speed;
	mView.y = mView.y + vOrthoVector.y * speed;
}