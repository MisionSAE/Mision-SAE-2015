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
 * @file	at24c5215.c
 * @author	SUR Emprendimientos Tecnologicos S.R.L.
 * @date	Mayo, 2015
 * @brief	Driver AT24C512 - Portable.
 *
 * Funciónes de la biblioteca de driver para memoria EEPROM AT24C512
 */

/*
 * at24c5215.h
 */

#ifndef AT24C512_H_
#define AT24C512_H_

/** \addtogroup Memoria
 *  @{
 */

/**
 * @addtogroup Funciones
 * @{
 */

/**
\msc
  hscale = "1";
  a [label="Aplicación"],b [label="Funciones"],c [label="Portables"] ,d [label="CIAA-Firmware"];
  ...;
  a abox d 		[label = "Inicialización del dispositivo"];
  a=>b 			[label = "AT24C512_Inicializar()"] ;
\endmsc
*/
/**
\msc
  hscale = "1";
  a [label="Aplicación"],b [label="Funciones"],c [label="Portables"] ,d [label="CIAA-Firmware"];
  ...;
  a abox d 		[label = "Procesos de usuario"];
  a=>b			[label = "AT24C512_Grabar()"];
  b=>c			[label = "AT24C512_I2C_Escribir()"];
  c=>d			[label = "ciaaPOSIX_write()"];
  d=>d			[label = "Acceso a Hardware"];
  c<=d;
  b<=c;
  a<=b;
\endmsc
*/
/**
\msc
  hscale = "1";
  a [label="Aplicación"],b [label="Funciones"],c [label="Portables"] ,d [label="CIAA-Firmware"];
  ...;
  a abox d		[label = "Procesos de usuario"];
  a=>b			[label = "AT24C512_Leer()"];
  b=>c			[label = "AT24C512_I2C_Leer()"];
  c=>d			[label = "ciaaPOSIX_read()"];
  d=>d			[label = "Acceso a Hardware"];
  c<=d;
  b<=c;
  a<=b;
\endmsc
*/


/**
 * @brief Conexión del pin AD0.
 *
 * Para selección la dirección del dispositivo
 *
 * @return
 */
#define _AT24C512_PIN_A__H		(1)
#define _AT24C512_PIN_A__L		(0)
#include <stdint.h>
#include "../inc/at24c512_config.h"

#define AT24C512_I2C_ADDRESS	(0x50 | (AT24C512_PIN_A1 << 1) | (AT24C512_PIN_A0 << 0))	///< Dirección en el BUS I2C

#include "../inc/at24c512_portable.h"

/**
 * @brief 	Inicializa el dispositio.
 *
 * Configura la dirección del registro de la memoria memoria externa
 * a leer/escribir en 0x0000.
 *
 * @return	1 en caso de éxitos, 0 en caso de falla.
 */
int AT24C512_Inicializar	(void);

/**
 * @brief 	Configura la dirección.
 *
 * Configura la dirección del registro en memoria a leer/escribir.
 *
 * @param 	[direccion]	La dirección del registro de la memoria externa.
 * @return	1 en caso de éxitos, 0 en caso de falla.
 */
int AT24C512_Direccion		(uint16_t direccion);

/**
 * @brief 	Graba los datos en la memoria.
 *
 * Graba en la memoria externa una cantidad datos especificada.
 * Se indica el origen de esos datos a través de su dirección en memoria.
 *
 * @param 	[pDatos]	La dirección de memoria donde se almacena el primer dato a grabar
 * @param 	[cantidad]	La cantidad de bytes a grabar.
 * @return	1 en caso de éxitos,
 * 			-1 en caso de falla en la comunicación,
 * 			0 en caso de otras fallas.
 */
int AT24C512_Grabar			(uint8_t * pDatos, uint8_t cantidad);

/**
 * @brief 	Leer los datos de la memoria.
 *
 * Lee de la memoria externa una cantidad datos especificada.
 * Se indica la posición de memoria donde se almacenan los datos leidos.
 *
 * @param 	[pDatos]	La dirección de memoria donde se almacenan los datos leidos.
 * @param 	[cantidad]	La cantidad de bytes a leer.
 * @return	1 en caso de éxitos,
 * 			-1 en caso de falla en la comunicación,
 * 			0 en caso de otras fallas.
 */
int AT24C512_Leer			(uint8_t * pDatos, uint8_t cantidad);

/** @}*/

/** @}*/

#endif /* AT24C512_H_ */
