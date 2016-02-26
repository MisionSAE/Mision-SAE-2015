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
 * mpu6050.h
 */

#ifndef MPU6050_H_
#define MPU6050_H_

/*******************************************
 * Registros
 *******************************************/
#define _MPU6050_SELF_TEST								(0x0d)

#define _MPU6050_ACCEL_CONFIG							(0x1c)
#define _MPU6050_ACCEL_CONFIG__XA_ST__H					(0x01 << 7)
#define _MPU6050_ACCEL_CONFIG__XA_ST__L					(0x00 << 7)
#define _MPU6050_ACCEL_CONFIG__YA_ST__H					(0x01 << 6)
#define _MPU6050_ACCEL_CONFIG__YA_ST__L					(0x00 << 6)
#define _MPU6050_ACCEL_CONFIG__ZA_ST__H					(0x01 << 5)
#define _MPU6050_ACCEL_CONFIG__ZA_ST__L					(0x00 << 5)
#define _MPU6050_ACCEL_CONFIG__AFS_SEL__2g				(0x00 << 3)
#define _MPU6050_ACCEL_CONFIG__AFS_SEL__4g				(0x01 << 3)
#define _MPU6050_ACCEL_CONFIG__AFS_SEL__8g				(0x02 << 3)
#define _MPU6050_ACCEL_CONFIG__AFS_SEL__16g				(0x03 << 3)

#define _MPU6050_ACCEL_XOUT_H							(0x3b)
#define _MPU6050_ACCEL_XOUT_L							(0x3c)
#define _MPU6050_ACCEL_YOUT_H							(0x3d)
#define _MPU6050_ACCEL_YOUT_L							(0x3e)
#define _MPU6050_ACCEL_ZOUT_H							(0x3f)
#define _MPU6050_ACCEL_ZOUT_L							(0x40)

#define _MPU6050_TEMP_OUT_H								(0x41)
#define _MPU6050_TEMP_OUT_L								(0x42)

#define _MPU6050_PWR_MGMT_1								(0x6b)
#define _MPU6050_PWR_MGMT_1__DEVICE_RESET__H			(0x01 << 7)
#define _MPU6050_PWR_MGMT_1__DEVICE_RESET__L			(0x00 << 7)
#define _MPU6050_PWR_MGMT_1__SLEEP__H					(0x01 << 6)
#define _MPU6050_PWR_MGMT_1__SLEEP__L					(0x00 << 6)
#define _MPU6050_PWR_MGMT_1__CYCLE__H					(0x01 << 5)
#define _MPU6050_PWR_MGMT_1__CYCLE__L					(0x00 << 5)
#define _MPU6050_PWR_MGMT_1__TEMP_DIS__H				(0x01 << 3)
#define _MPU6050_PWR_MGMT_1__TEMP_DIS__L				(0x00 << 3)
#define _MPU6050_PWR_MGMT_1__CLKSEL__0					(0x00 << 0)
#define _MPU6050_PWR_MGMT_1__CLKSEL__1					(0x01 << 0)
#define _MPU6050_PWR_MGMT_1__CLKSEL__2					(0x02 << 0)
#define _MPU6050_PWR_MGMT_1__CLKSEL__3					(0x03 << 0)
#define _MPU6050_PWR_MGMT_1__CLKSEL__4					(0x04 << 0)
#define _MPU6050_PWR_MGMT_1__CLKSEL__5					(0x05 << 0)
#define _MPU6050_PWR_MGMT_1__CLKSEL__6					(0x06 << 0)
#define _MPU6050_PWR_MGMT_1__CLKSEL__7					(0x07 << 0)


/*******************************************
 * Coneccion del pin AD0: provoca un cambio en la dirección del I2C BUS
 *******************************************/
#define _MPU6050_PIN_AD0__H		(1)
#define _MPU6050_PIN_AD0__L		(0)

#define _MPU6050_X				(0)
#define _MPU6050_Y				(1)
#define _MPU6050_Z				(2)

/*******************************************
 * Configuración del usuario
 *******************************************/
#include <stdint.h>
#include <math.h>
#include "mpu6050_config.h"

/*******************************************
 *
 *******************************************/
#if MPU6050_PIN_AD0 == _MPU6050_PIN_AD0__H
#define MPU6050_I2C_ADDRESS		(0x69)
#else
#define MPU6050_I2C_ADDRESS		(0x68)
#endif

#include "mpu6050_portable.h"

int 	MPU6050_Reset						(void);
int 	MPU6050_Inicializar					(void);
int 	MPU6050_Medir						(void);
float 	MPU6050_LeerAceleracionX			(void);
float 	MPU6050_LeerAceleracionY			(void);
float 	MPU6050_LeerAceleracionZ			(void);
float 	MPU6050_LeerTemperatura				(void);
int 	MPU6050__LeerRegistroEscala			(void);
int 	MPU6050__LeerRegistroAceleracion	(void);
int 	MPU6050__LeerRegistroTemperatura	(void);
int 	MPU6050__ProcesarLecturas			(void);
float 	MPU6050__CalculoFT					(uint8_t test);
int 	MPU6050__LecturaFT					(void);
void 	MPU6050_Valor_RegistroAceleracion	(int8_t * pValor);
void 	MPU6050_Valor_RegistroEscala		(int8_t * pValor);

#endif /* MPU6050_H_ */
