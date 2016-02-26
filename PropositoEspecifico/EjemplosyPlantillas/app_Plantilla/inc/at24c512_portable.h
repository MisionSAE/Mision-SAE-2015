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
 * @file	at24c5215_portable.h
 * @author	SUR Emprendimientos Tecnologicos S.R.L.
 * @date	Mayo, 2015
 * @brief	Driver AT24C512 - Portable.
 *
 * Funciónes portables de la biblioteca de driver para memoria EEPROM AT24C512
 */

/*
 * at24c5215_portable.h
 */

#ifndef AT24C512_PORTABLE_H_
#define AT24C512_PORTABLE_H_

/** \addtogroup Memoria
 *  @{
 */

#include "../inc/at24c512.h"
#include "ciaaPOSIX_stdio.h"
#include "ciaaPOSIX_string.h"
#include "../inc/dispositivos.h"

/** \addtogroup Portable
 *  @{
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
int AT24C512_I2C_Escribir(uint8_t addr, uint8_t *pBuf, uint32_t len);

/**
 * @brief 	Lee del BUS I2C
 *
 * Lee del BUS I2C una cantidad de bytes determinada
 * a partir de la posición de memoria indicada.
 *
 * @param 	[addr] 	La dirección dentro del BUS I2C del dispositivo.
 * @param 	[pBuf_out] 	Puntero al origen de datos a escribir en el BUS I2C.
 * @param 	[len_out] 	La longitud de trama a escribir en el BUS I2C.
 * @param 	[pBuf_in] 	Puntero al origen de datos a leer en el BUS I2C.
 * @param 	[len_in] 	La longitud de trama a leer en el BUS I2C.
 * @return	La longitud de trama que escribio en el BUS I2C.
 */
int AT24C512_I2C_Leer(uint8_t addr, uint8_t *pBuf_out, uint32_t len_out, uint8_t *pBuf_in, uint32_t len_in);

/** @}*/

/** @}*/

#endif /* AT24C512_PORTABLE_H_ */
