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
 * bmp085.h
 */

#ifndef BMP085_H_
#define BMP085_H_

/** \addtogroup Memoria
 *  @{
 */

#define _BMP085_REG								(0xff)
#define _BMP085_REG_AC1__MSB					(0xaa)					///< Dirección del registro, parte alta
#define _BMP085_REG_AC1__LSB					(0xab)					///< Dirección del registro, parte baja
#define _BMP085_REG_AC2__MSB					(0xac)					///< Dirección del registro, parte alta
#define _BMP085_REG_AC2__LSB					(0xad)					///< Dirección del registro, parte baja
#define _BMP085_REG_AC3__MSB					(0xae)					///< Dirección del registro, parte alta
#define _BMP085_REG_AC3__LSB					(0xaf)					///< Dirección del registro, parte baja
#define _BMP085_REG_AC4__MSB					(0xb0)					///< Dirección del registro, parte alta
#define _BMP085_REG_AC4__LSB					(0xb1)					///< Dirección del registro, parte baja
#define _BMP085_REG_AC5__MSB					(0xb2)					///< Dirección del registro, parte alta
#define _BMP085_REG_AC5__LSB					(0xb3)					///< Dirección del registro, parte baja
#define _BMP085_REG_AC6__MSB					(0xb4)					///< Dirección del registro, parte alta
#define _BMP085_REG_AC6__LSB					(0xb5)					///< Dirección del registro, parte baja
#define _BMP085_REG_B1__MSB						(0xb6)					///< Dirección del registro, parte alta
#define _BMP085_REG_B1__LSB						(0xb7)					///< Dirección del registro, parte baja
#define _BMP085_REG_B2__MSB						(0xb8)					///< Dirección del registro, parte alta
#define _BMP085_REG_B2__LSB						(0xb9)					///< Dirección del registro, parte baja
#define _BMP085_REG_MB__MSB						(0xba)					///< Dirección del registro, parte alta
#define _BMP085_REG_MB__LSB						(0xbb)					///< Dirección del registro, parte baja
#define _BMP085_REG_MC__MSB						(0xbc)					///< Dirección del registro, parte alta
#define _BMP085_REG_MC__LSB						(0xbd)					///< Dirección del registro, parte baja
#define _BMP085_REG_MD__MSB						(0xbe)					///< Dirección del registro, parte alta
#define _BMP085_REG_MD__LSB						(0xbf)					///< Dirección del registro, parte baja

#define _BMP085_REG_READSET						(0xf4)					///< Comando de lectura
#define _BMP085_REG_READSET__TEMPERATURE		(0x2e)					///< Selección de lectura de temperatura
#define _BMP085_REG_READSET__PRESSURE(oss)		(0x34 + ((oss) << 6))	///< Selección de lectura de presión según resolución oss

#define _BMP085_REG_MEASURE__MSB				(0xf6)					///< Dirección del registro, bits[23:16]
#define _BMP085_REG_MEASURE__LSB				(0xf7)					///< Dirección del registro, bits[15:8]
#define _BMP085_REG_MEASURE__XLSB				(0xf8)					///< Dirección del registro, bits[7:0|

/*******************************************
 * Configuración del usuario
 *******************************************/
#include <stdint.h>

#include "bmp085_config.h"

#define BMP085_I2C_ADDRESS		(0x77)

#include "bmp085_portable.h"

/** \addtogroup Funciones
 *  @{
 */

/**
\msc
  hscale = "1";
  a [label="Aplicación"],b [label="Funciones"],c [label="Portables"] ,d [label="CIAA-Firmware"];
  ...;
  a abox d		[label = "Inicialización del dispositivo"];
  a=>b			[label = "BMP085_Inicializar()"] ;
  b=>b			[label = "BMP085_LeerCoeficientes()"];
  b=>c			[label = "BMP085_I2C_Leer()"];
  c=>d			[label = "ciaaPOSIX_read()"];
  d=>d			[label = "Acceso a Hardware"];
  c<=d;
  b<=c;
\endmsc
*/
/**
\msc
  hscale = "1";
  a [label="Aplicación"],b [label="Funciones"],c [label="Portables"] ,d [label="CIAA-Firmware"];
  ...;
  a abox d		[label = "Proceso de lectura de presión"];
  a=>b			[label = "BMP085_Presion_Medir()"];
  b=>c			[label = "BMP085_I2C_Escribir()"];
  c=>d			[label = "ciaaPOSIX_write()"];
  d=>d			[label = "Acceso a Hardware"];
  ---			[label = "Demora propia del dispositivo"];
  a=>b			[label = "BMP085_Presion_LeerMedicion()"];
  b=>c			[label = "BMP085_I2C_Leer()"];
  c=>d			[label = "ciaaPOSIX_read()"];
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
  a abox d		[label = "Proceso de lectura de temperatura"];
  a=>b			[label = "BMP085_Temperatura_Medir()"];
  b=>c			[label = "BMP085_I2C_Escribir()"];
  c=>d			[label = "ciaaPOSIX_write()"];
  d=>d			[label = "Acceso a Hardware"];
  ---			[label = "Demora propia del dispositivo"];
  a=>b			[label = "BMP085_Temperatura_LeerMedicion()"];
  b=>c			[label = "BMP085_I2C_Leer()"];
  c=>d			[label = "ciaaPOSIX_read()"];
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
  a abox d		[label = "Proceso de mediciones"];
  a=>b			[label = "BMP085_ProcesarLecturas()"];
  a<=b;
  a=>b			[label = "BMP085_LeerTemperatura()"];
  a<=b;
  a=>b			[label = "BMP085_LeerPresion()"];
  a<=b;
\endmsc
*/

/**
 * @brief 	Reset de software del dispositio.
 *
 * Genera un reset de software en el dispositivo
 *
 * @return	1 en caso de éxitos.
 */
int BMP085_Reset			(void);

/**
 * @brief 	Inicializa el dispositio.
 *
 * Inicializa el dispositio.
 * Lee los coeficiente de calibración del dispositivo.
 *
 * @return	1 en caso de éxitos, -1 en caso de falla.
 */
int 	BMP085_Inicializar					(void);

//int 	BMP085_LeerRegistrosCoeficientes	(void);

//int 	BMP085_LeerRegistroTemperatura		(void);

//int 	BMP085_LeerRegistroPresion			(void);

/**
 * @brief 	Procesa los datos de temperatura y presión.
 *
 * Procesa los datos de temperatura y presión según las correcciones sugeridas por el fabricante.
 * Utiliza los coeficientes de calibración.
 *
 * @return	1 en caso de éxitos, -1 en caso de falla.
 */
int 	BMP085_ProcesarLecturas				(void);

//int 	BMP085_Medir						(void);

/**
 * @brief 	Devuelve el valor de temperatura.
 *
 * Devuelve el valor de temperatura en unidades del fabricante.
 *
 * @return	valor de temperatura.
 */
float 	BMP085_LeerTemperatura				(void);

/**
 * @brief 	Devuelve el valor de presión.
 *
 * Devuelve el valor de presión en unidades del fabricante.
 *
 * @return	valor de presión.
 */
float 	BMP085_LeerPresion					(void);

/**
 * @brief 	Carga valores de prueba para verificar algoritmo de procesamiento.
 *
 * Carga valores de prueba de coeficientes de calibración, sugeridos por fabricante,
 * para verificar algoritmo de procesamiento.
 *
 * @return	1 en caso de éxitos, -1 en caso de falla.
 */
int 	BMP085_Prueba_CargarValores			(void);

/**
 * @brief 	Copia el valor de registro del dispositivo en memoria.
 *
 * Copia el valor de los registro de calibración leidos del dispositivo
 * en una posición de memoria determinada.
 *
 * @param 	[pValor]	La dirección en memoria donde se copiarán lo datos.
 */
void 	BMP085_Valor_RegistroCalibracion	(int8_t * pValor);

/**
 * @brief 	Copia el valor de registro del dispositivo en memoria.
 *
 * Copia el valor de los registro de temperatura sin corrección leidos del dispositivo
 * en una posición de memoria determinada.
 *
 * @param 	[pValor]	La dirección en memoria donde se copiarán lo datos.
 */
void 	BMP085_Valor_RegistroTemperatura	(int8_t * pValor);

/**
 * @brief 	Copia el valor de registro del dispositivo en memoria.
 *
 * Copia el valor de los registro de presión sin corrección leidos del dispositivo
 * en una posición de memoria determinada.
 *
 * @param 	[pValor]	La dirección en memoria donde se copiarán lo datos.
 */
void 	BMP085_Valor_RegistroPresion		(int8_t * pValor);

/**
 * @brief 	Envia el comando de medición al dispositivo.
 *
 * Envia el comando de medición de temperatura al dispositivo.
 *
 * @return	1 en caso de éxitos, -1 en caso de falla.
 */
int 	BMP085_Temperatura_Medir			(void);

/**
 * @brief 	Lee el valor de la medición.
 *
 * Lee el valor de la medición de temperatura sin corrección
 * en los registros del dispositivo.
 *
 * @return	1 en caso de éxitos, -1 en caso de falla.
 */
int 	BMP085_Temperatura_LeerMedicion		(void);

/**
 * @brief 	Envia el comando de medición al dispositivo.
 *
 * Envia el comando de medición de presión al dispositivo.
 *
 * @return	1 en caso de éxitos, -1 en caso de falla.
 */
int 	BMP085_Presion_Medir				(void);

/**
 * @brief 	Lee el valor de la medición.
 *
 * Lee el valor de la medición de presión sin corrección
 * en los registros del dispositivo.
 *
 * @return	1 en caso de éxitos, -1 en caso de falla.
 */
int 	BMP085_Presion_LeerMedicion			(void);

/** @} */

/** @} */

#endif /* BMP085_H_ */
