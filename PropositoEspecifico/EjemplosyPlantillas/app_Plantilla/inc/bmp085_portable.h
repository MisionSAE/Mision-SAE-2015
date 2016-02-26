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

/**
 * @file	bmp085_portable.h
 * @author	SUR Emprendimientos Tecnologicos S.R.L.
 * @date	Mayo, 2015
 * @brief	Driver AT24C512 - Portable.
 *
 * Funciónes portables de la biblioteca de driver para memoria EEPROM AT24C512
 */

/*
 * bmp085_portable.c
 */

#ifndef BMP085_PORTABLE_H_
#define BMP085_PORTABLE_H_

/**
 * @addtogroup Barómetro
 * @{
 */

#include "../inc/bmp085.h"
#include "ciaaPOSIX_stdio.h"
#include "ciaaPOSIX_string.h"
#include "../inc/dispositivos.h"

/**
 * @addtogroup Portable
 * @{
 */

/**
 * @brief 	Escribe en el BUS I2C
 *
 * Escribe en el BUS I2C una cantidad de bytes determinada
 * a partir de la posición de memoria indicada.
 *
 * @param 	[addr] 	La dirección dentro del BUS I2C del dispositivo.
 * @param 	[pBuf] 	Puntero al origen de datos a escribir en el BUS I2C.
 * @param 	[len] 	La longitud de trama a escribir en el BUS I2C.
 * @return	La longitud de trama que escribio en el BUS I2C.
 */
int BMP085_I2C_Escribir		(uint8_t addr, uint8_t *pBuf, uint32_t len);

/**
 * @brief 	Lee del BUS I2C
 *
 * Lee del BUS I2C una cantidad de bytes determinada
 * a partir de la posición de memoria indicada.
 *
 * @param 	[addr] 	La dirección dentro del BUS I2C del dispositivo.
 * @param 	[pBuf] 	Puntero al origen de datos a escribir en el BUS I2C.
 * @param 	[len] 	La longitud de trama a escribir en el BUS I2C.
 * @return	La longitud de trama que escribio en el BUS I2C.
 */
int BMP085_I2C_Leer			(uint8_t addr, uint8_t *buf_out, uint32_t len_out, uint8_t *buf_in, uint32_t len_in);

/** @}*/

/** @}*/

#endif /* BMP085_PORTABLE_H_ */
