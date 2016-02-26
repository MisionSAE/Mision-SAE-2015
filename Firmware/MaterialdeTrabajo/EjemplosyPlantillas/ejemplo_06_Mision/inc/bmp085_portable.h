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

/*
 * bmp085_portable.c
 */

#ifndef BMP085_PORTABLE_H_
#define BMP085_PORTABLE_H_
#include "../inc/bmp085.h"
#include "ciaaPOSIX_stdio.h"
#include "ciaaPOSIX_string.h"
#include "../inc/dispositivos.h"
/*******************************************
 * El usuario debe crear las dos funciones
 *******************************************/
int BMP085_I2C_Escribir		(uint8_t addr, uint8_t *pBuf, uint32_t len);
int BMP085_I2C_Leer			(uint8_t addr, uint8_t *buf_out, uint32_t len_out, uint8_t *buf_in, uint32_t len_in);
#endif /* BMP085_PORTABLE_H_ */
