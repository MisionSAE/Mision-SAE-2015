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
 * mpu6050.h
 */

#ifndef MPU6050_H_
#define MPU6050_H_

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
  a abox d		[label = "Inicialización del dispositivo"];
  a=>b			[label = "MPU6050_Reset()"] ;
  b=>c			[label = "MPU6050_I2C_Escribir()"];
  c=>d			[label = "ciaaPOSIX_write()"];
  d=>d			[label = "Acceso a Hardware"];
  ---			[label = "Demora"];
  a=>b			[label = "MPU6050_Inicializar()"] ;
  b=>c			[label = "MPU6050_I2C_Escribir()"];
  c=>d			[label = "ciaaPOSIX_write()"];
  d=>d			[label = "Acceso a Hardware"];
  ---			[label = "Demora"];
  a=>b			[label = "MPU6050_ConfigurarAcelerometro()"] ;
  b=>c			[label = "MPU6050_I2C_Escribir()"];
  c=>d			[label = "ciaaPOSIX_write()"];
  d=>d			[label = "Acceso a Hardware"];
\endmsc
*/
/**
\msc
  hscale = "1";
  a [label="Aplicación"],b [label="Funciones"],c [label="Portables"] ,d [label="CIAA-Firmware"];
  ...;
  a abox d		[label = "Proceso de medición"];
  a=>b			[label = "MPU6050_Medir()"];
  b=>b			[label = "MPU6050__LeerRegistroEscala()"];
  b=>c			[label = "MPU6050_I2C_Leer()"];
  c=>d			[label = "ciaaPOSIX_read()"];
  d=>d			[label = "Acceso a Hardware"];
  c<=d;
  b<=c;
  b=>b			[label = "MPU6050__LeerRegistroAceleracion()"];
  b=>c			[label = "MPU6050_I2C_Leer()"];
  c=>d			[label = "ciaaPOSIX_read()"];
  d=>d			[label = "Acceso a Hardware"];
  c<=d;
  b<=c;
  b=>b			[label = "MPU6050__ProcesarLecturas()"];
  a<=b;
\endmsc
*/
/**
\msc
  hscale = "1";
  a [label="Aplicación"],b [label="Funciones"],c [label="Portables"] ,d [label="CIAA-Firmware"];
  ...;
  a abox d		[label = "Proceso de lectura de valores medidos"];
  a=>b			[label = "MPU6050_LeerAceleracionX()"];
  a<=b;
  a=>b			[label = "MPU6050_LeerAceleracionY()"];
  a<=b;
  a=>b			[label = "MPU6050_LeerAceleracionZ()"];
  a<=b;
  a=>b			[label = "MPU6050_LeerTemperatura()"];
  a<=b;
\endmsc
*/
/**
\msc
  hscale = "1";
  a [label="Aplicación"],b [label="Funciones"],c [label="Portables"] ,d [label="CIAA-Firmware"];
  ...;
  a abox d		[label = "Proceso de lectura de valores de registros"];
  a=>b			[label = "MPU6050_Valor_RegistroAceleracion()"];
  b=>d			[label = "ciaaPOSIX_memcpy()"];
  b<=d;
  a<=b;
  a=>b			[label = "MPU6050_Valor_RegistroEscala()"];
  b=>d			[label = "ciaaPOSIX_memcpy()"];
  b<=d;
  a<=b;
\endmsc
*/

#define _MPU6050_SELF_TEST								(0x0d)			///< Dirección del registro

#define _MPU6050_ACCEL_CONFIG							(0x1c)			///< Dirección del registro
#define _MPU6050_ACCEL_CONFIG__XA_ST__H					(0x01 << 7)		///< Activa auto test en eje X
#define _MPU6050_ACCEL_CONFIG__XA_ST__L					(0x00 << 7)		///< Desactiva auto test en eje X
#define _MPU6050_ACCEL_CONFIG__YA_ST__H					(0x01 << 6)		///< Activa auto test en eje Y
#define _MPU6050_ACCEL_CONFIG__YA_ST__L					(0x00 << 6)		///< Desactiva auto test en eje Y
#define _MPU6050_ACCEL_CONFIG__ZA_ST__H					(0x01 << 5)		///< Activa auto test en eje Z
#define _MPU6050_ACCEL_CONFIG__ZA_ST__L					(0x00 << 5)		///< Desactiva auto test en eje Z
#define _MPU6050_ACCEL_CONFIG__AFS_SEL__2g				(0x00 << 3)		///< Selecciona escala +-2g
#define _MPU6050_ACCEL_CONFIG__AFS_SEL__4g				(0x01 << 3)		///< Selecciona escala +-4g
#define _MPU6050_ACCEL_CONFIG__AFS_SEL__8g				(0x02 << 3)		///< Selecciona escala +-8g
#define _MPU6050_ACCEL_CONFIG__AFS_SEL__16g				(0x03 << 3)		///< Selecciona escala +-16g

#define _MPU6050_ACCEL_XOUT_H							(0x3b)			///< Dirección de la parte alta del valor de aceleración en eje X
#define _MPU6050_ACCEL_XOUT_L							(0x3c)			///< Dirección de la parte baja del valor de aceleración en eje X
#define _MPU6050_ACCEL_YOUT_H							(0x3d)			///< Dirección de la parte alta del valor de aceleración en eje Y
#define _MPU6050_ACCEL_YOUT_L							(0x3e)			///< Dirección de la parte baja del valor de aceleración en eje Y
#define _MPU6050_ACCEL_ZOUT_H							(0x3f)			///< Dirección de la parte alta del valor de aceleración en eje Z
#define _MPU6050_ACCEL_ZOUT_L							(0x40)			///< Dirección de la parte baja del valor de aceleración en eje Z

#define _MPU6050_TEMP_OUT_H								(0x41)			///< Dirección de la parte alta del valor de temperatura
#define _MPU6050_TEMP_OUT_L								(0x42)			///< Dirección de la parte baja del valor de temperatura

#define _MPU6050_PWR_MGMT_1								(0x6b)			///< Dirección del registro
#define _MPU6050_PWR_MGMT_1__DEVICE_RESET__H			(0x01 << 7)		///< Activa reset por software
#define _MPU6050_PWR_MGMT_1__DEVICE_RESET__L			(0x00 << 7)		///< Desactiva reset por software
#define _MPU6050_PWR_MGMT_1__SLEEP__H					(0x01 << 6)		///< Activa modo bajo consumo
#define _MPU6050_PWR_MGMT_1__SLEEP__L					(0x00 << 6)		///< Desactiva modo bajo consumo
#define _MPU6050_PWR_MGMT_1__CYCLE__H					(0x01 << 5)		///< Activa modo de conversión continua
#define _MPU6050_PWR_MGMT_1__CYCLE__L					(0x00 << 5)		///< Desactiva modo de conversión continua
#define _MPU6050_PWR_MGMT_1__TEMP_DIS__H				(0x01 << 3)		///< Desactiva el sensor de temperatura
#define _MPU6050_PWR_MGMT_1__TEMP_DIS__L				(0x00 << 3)		///< Activa el sensor de temperatura
/**
 * @brief Seleccionan la fuente de reloj
 */
#define _MPU6050_PWR_MGMT_1__CLKSEL__0					(0x00 << 0)
#define _MPU6050_PWR_MGMT_1__CLKSEL__1					(0x01 << 0)
#define _MPU6050_PWR_MGMT_1__CLKSEL__2					(0x02 << 0)
#define _MPU6050_PWR_MGMT_1__CLKSEL__3					(0x03 << 0)
#define _MPU6050_PWR_MGMT_1__CLKSEL__4					(0x04 << 0)
#define _MPU6050_PWR_MGMT_1__CLKSEL__5					(0x05 << 0)
#define _MPU6050_PWR_MGMT_1__CLKSEL__6					(0x06 << 0)
#define _MPU6050_PWR_MGMT_1__CLKSEL__7					(0x07 << 0)

#define _MPU6050_PIN_AD0__H		(1)
#define _MPU6050_PIN_AD0__L		(0)

#define _MPU6050_X				(0)
#define _MPU6050_Y				(1)
#define _MPU6050_Z				(2)

#include <stdint.h>
#include <math.h>
#include "mpu6050_config.h"

#if MPU6050_PIN_AD0 == _MPU6050_PIN_AD0__H
#define MPU6050_I2C_ADDRESS		(0x69)
#else
#define MPU6050_I2C_ADDRESS		(0x68)
#endif

#include "mpu6050_portable.h"

/**
 * @brief 	Fuerza un reset de software en el dispositio.
 *
 * @return	1 en caso de éxitos, -1 en caso de falla.
 */
int 	MPU6050_Reset						(void);

 /**
  * @brief 	Inicializa el dispositio.
  *
  * Desactiva el modo bajo consumo.
  *
  * @return	1 en caso de éxitos, -1 en caso de falla.
  */
int 	MPU6050_Inicializar					(void);

/**
 * @brief 	Configura parametros del acelerometro.
 *
 * @param	[selftest]		Desactiva el modo auto test si su valor es 0.
 * @param	[afs_sel]		Selecciona el rango de fondo de escala.
 * 							- 0 : de -2g a +2g
 * 							- 1 : de -4g a +4g
 * 							- 2 : de -8g a +8g
 * 							- 3 : de -16g a +16g
 * @return	1 en caso de éxitos, -1 en caso de falla.
 */
int		MPU6050_ConfigurarAcelerometro		(uint8_t selftest, uint8_t afs_sel);

/**
 * @brief 	Realiza una lectura del valor actual de aceleración
 *
 * @return	0 en caso de éxitos.
 */
int 	MPU6050_Medir						(void);

/**
 * @brief 	Devuelve el valor de aceleracion medido en el eje X.
 *
 * @return	Valor de aceleración en el eje X.
 */
float 	MPU6050_LeerAceleracionX			(void);

/**
 * @brief 	Devuelve el valor de aceleracion medido en el eje Y.
 *
 * @return	Valor de aceleración en el eje Y.
 */
float 	MPU6050_LeerAceleracionY			(void);

/**
 * @brief 	Devuelve el valor de aceleracion medido en el eje Z.
 *
 * @return	Valor de aceleración en el eje Z.
 */
float 	MPU6050_LeerAceleracionZ			(void);

/**
 * @brief 	Devuelve el valor de temperatura interna del integrado.
 *
 * @return	Valor de temperatura.
 */
float 	MPU6050_LeerTemperatura				(void);

/**
 * @brief 	Lee el valor de fondo de escala del dispositivo.
 *
 * @return	1 en caso de éxitos, -1 en caso de falla.
 */
int 	MPU6050__LeerRegistroEscala			(void);

/**
 * @brief 	Lee el valor de aceleración actual en los ejes X, Y y Z del dispositivo.
 *
 * @return	1 en caso de éxitos, -1 en caso de falla.
 */
int 	MPU6050__LeerRegistroAceleracion	(void);

/**
 * @brief 	Lee el valor de temperatura interna del integrado.
 *
 * @return	1 en caso de éxitos, -1 en caso de falla.
 */
int 	MPU6050__LeerRegistroTemperatura	(void);

/**
 * @brief 	Procesa las lecturas de aceleración y temperatura
 *
 * Convierte las lecturas de aceleración en unidades de gravedad.
 * Convierte la lectura de temepratura en unidades grados centrigrados.
 *
 * @return	1 en caso de éxitos.
 */
int 	MPU6050__ProcesarLecturas			(void);

//float 	MPU6050__CalculoFT					(uint8_t test);

//int 	MPU6050__LecturaFT					(void);

/**
 * @brief 	Copia el valor de registro del dispositivo en memoria.
 *
 * Copia el valor de los registro de aceleracion leidos del dispositivo
 * en una posición de memoria determinada.
 *
 * @param 	[pValor]	La dirección en memoria donde se copiarán lo datos.
 */
void 	MPU6050_Valor_RegistroAceleracion	(int8_t * pValor);

/**
 * @brief 	Copia el valor de registro del dispositivo en memoria.
 *
 * Copia el valor de del registro de escala leido del dispositivo
 * en una posición de memoria determinada.
 *
 * @param 	[pValor]	La dirección en memoria donde se copiarán lo datos.
 */
void 	MPU6050_Valor_RegistroEscala		(int8_t * pValor);

/** @} */

/** @} */

#endif /* MPU6050_H_ */
