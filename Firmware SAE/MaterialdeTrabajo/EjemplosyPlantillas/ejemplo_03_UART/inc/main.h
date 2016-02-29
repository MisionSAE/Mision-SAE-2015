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

#ifndef _MAIN_H_
#define _MAIN_H_

#include "plataforma.h"
#define BASETIEMPO	(10) // 10[milisegundos] = 0.010[segundos]

int LeerEntradas		(void);
int Procesar			(void);
int EscribirSalidas		(void);

#endif /* #ifndef _MAIN_H_ */
