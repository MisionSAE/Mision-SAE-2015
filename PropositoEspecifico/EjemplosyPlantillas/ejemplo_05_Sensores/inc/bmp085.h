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
 * bmp085.h
 */

#ifndef BMP085_H_
#define BMP085_H_

/*******************************************
 * Registros
 *******************************************/
#define _BMP085_REG							(0xff)
#define _BMP085_REG_AC1__MSB					(0xaa)
#define _BMP085_REG_AC1__LSB					(0xab)
#define _BMP085_REG_AC2__MSB					(0xac)
#define _BMP085_REG_AC2__LSB					(0xad)
#define _BMP085_REG_AC3__MSB					(0xae)
#define _BMP085_REG_AC3__LSB					(0xaf)
#define _BMP085_REG_AC4__MSB					(0xb0)
#define _BMP085_REG_AC4__LSB					(0xb1)
#define _BMP085_REG_AC5__MSB					(0xb2)
#define _BMP085_REG_AC5__LSB					(0xb3)
#define _BMP085_REG_AC6__MSB					(0xb4)
#define _BMP085_REG_AC6__LSB					(0xb5)
#define _BMP085_REG_B1__MSB						(0xb6)
#define _BMP085_REG_B1__LSB						(0xb7)
#define _BMP085_REG_B2__MSB						(0xb8)
#define _BMP085_REG_B2__LSB						(0xb9)
#define _BMP085_REG_MB__MSB						(0xba)
#define _BMP085_REG_MB__LSB						(0xbb)
#define _BMP085_REG_MC__MSB						(0xbc)
#define _BMP085_REG_MC__LSB						(0xbd)
#define _BMP085_REG_MD__MSB						(0xbe)
#define _BMP085_REG_MD__LSB						(0xbf)

#define _BMP085_REG_READSET						(0xf4)
#define _BMP085_REG_READSET__TEMPERATURE		(0x2e)
#define _BMP085_REG_READSET__PRESSURE(oss)		(0x34 + ((oss) << 6))

#define _BMP085_REG_MEASURE__MSB				(0xf6)
#define _BMP085_REG_MEASURE__LSB				(0xf7)
#define _BMP085_REG_MEASURE__XLSB				(0xf8)

/*******************************************
 * Configuración del usuario
 *******************************************/
#include <stdint.h>

#include "bmp085_config.h"

/*******************************************
 * Portación a plataforma
 *******************************************/
#define BMP085_I2C_ADDRESS		(0x77)

#include "bmp085_portable.h"

/*******************************************
 * Funciones no recomendadas para usar de forma externa a la biblioteca
*******************************************/
int BMP085_Reset			(void);

/*******************************************
 * Funciones recomedadas
 *******************************************/
int 	BMP085_Inicializar					(void);
int 	BMP085_LeerRegistrosCoeficientes	(void);
int 	BMP085_LeerRegistroTemperatura		(void);
int 	BMP085_LeerRegistroPresion			(void);
int 	BMP085_ProcesarLecturas				(void);
int 	BMP085_Medir						(void);
float 	BMP085_LeerTemperatura				(void);
float 	BMP085_LeerPresion					(void);
int 	BMP085_Prueba_CargarValores			(void);
void 	BMP085_Valor_RegistroCalibracion	(int8_t * pValor);
void 	BMP085_Valor_RegistroTemperatura	(int8_t * pValor);
void 	BMP085_Valor_RegistroPresion		(int8_t * pValor);

int 	BMP085_Temperatura_Medir			(void);
int 	BMP085_Temperatura_LeerMedicion		(void);
int 	BMP085_Presion_Medir				(void);
int 	BMP085_Presion_LeerMedicion			(void);

#endif /* BMP085_H_ */
