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

#ifndef _DEFINICIONES_H_
#define _DEFINICIONES_H_

/***********************************************************************
 *
 ***********************************************************************/

#include "os.h"
#include "ciaaPOSIX_stdio.h"
#include "ciaaPOSIX_stdlib.h"
#include "ciaaPOSIX_string.h"
#include "ciaak.h"

/***********************************************************************
 *
 ***********************************************************************/

#define SAE_OK				(1)

/*
 * TECLAS y LEDS
 */
#define	TECLA_1						(1<<0)
#define	TECLA_2						(1<<1)
#define	TECLA_3						(1<<2)
#define	TECLA_4						(1<<3)
#define	LED_0_R 					(1<<0)
#define	LED_0_G 					(1<<1)
#define	LED_0_B 					(1<<2)
#define	LED_1						(1<<3)
#define	LED_2						(1<<4)
#define	LED_3						(1<<5)
#define	LED_TODOS					(0x3f)

char* sprintf_float(char* buffer, const char* formato, float dato);

#define DEBUG_OUT(x)				ciaaPOSIX_write(uartUSB, (x), ciaaPOSIX_strlen(x))
#define RADIO_OUT(x)				XBeeTransmitirDatos((x), ciaaPOSIX_strlen((x)))
#define UARTS_OUT(x)				DEBUG_OUT(x); \
									RADIO_OUT(x)

#define GRAVEDAD 				(9.81)

#endif /* #ifndef _DEFINICIONES_H_ */
