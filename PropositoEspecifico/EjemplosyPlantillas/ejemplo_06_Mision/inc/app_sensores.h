/**************************************************************************
 *  Desarrollado por SUR Emprendimientos Tecnologicos S.R.L.
 *  Email: contacto@suremptec.com.ar
 *  Web: http://www.suremptec.com
 **************************************************************************
 *
 * Firmware SAE (c) 2015 CONAE
 *
 **************************************************************************
 * Esta obra se encuentra protegida por derechos de autor, queda prohibida
 * su reproducción, modificacion y/o publicacion, por cualquier medio,
 * sin autorizacion previa de CONAE.
 ***************************************************************************/

#ifndef _APP_SENSORES_H_
#define _APP_SENSORES_H_

/***********************************************************************
 *
 ***********************************************************************/

#include "../inc/plataforma.h"

/***********************************************************************
 *
 ***********************************************************************/

#define N_COEFICIENTES			(22)

/***********************************************************************
 *
 ***********************************************************************/

typedef struct sensores_s
{
	uint8_t 	estado;
	uint8_t		escala;
	uint8_t		coeficientes[N_COEFICIENTES];
	uint16_t	direccion;
} sensores_u;

/***********************************************************************
 *
 ***********************************************************************/

extern sensores_u sensores;

/***********************************************************************
 *
 ***********************************************************************/

enum
{
	SENSORES_INICIALIZANDO,
	SENSORES_OK,
	SENSORES_ERROR,
};

/***********************************************************************
 *
 ***********************************************************************/

int Sensores_Inicializar(int *);

#endif /* #ifndef _APP_SENSORES_H_ */
