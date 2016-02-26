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

#ifndef _APP_MEDICION_H_
#define _APP_MEDICION_H_

/***********************************************************************
 *
 ***********************************************************************/

#include "../inc/plataforma.h"

/***********************************************************************
 *
 ***********************************************************************/

typedef struct medicion_dato_s
{
	float 				temperatura;
	float 				presion;
	float 				acel[3];
	float				temp;
} medicion_dato_u;

typedef struct medicion_s
{
	uint8_t 			estado;
	bool				estado_barometro;
	bool				estado_acelerometro;
	medicion_dato_u		dato;
	uint16_t			direccion_dato_base;
} medicion_u;

/***********************************************************************
 *
 ***********************************************************************/

extern medicion_u medicion;

/***********************************************************************
 *
 ***********************************************************************/

int Medicion(void);

/***********************************************************************
 *
 ***********************************************************************/

enum
{
	MEDICION_PROCESANDO,
	MEDICION_OK,
	MEDICION_ERROR,
};

#endif /* #ifndef _APP_MEDICION_H_ */
