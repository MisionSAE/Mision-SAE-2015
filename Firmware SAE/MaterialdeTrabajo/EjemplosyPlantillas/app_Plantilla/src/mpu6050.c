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
 * mpu6050.c
 */

/**
 * @addtogroup Memoria
 * @{
 */

/**
 * @addtogroup Funciones
 * @{
 */

#include "../inc/mpu6050.h"

/**
 * @brief	Estructura de abstracción del dispositivo AT24C512
 */
typedef struct
{
	float	aceleracion[3];
	float	temperatura;
	int16_t escala;
	int8_t 	reg_escala;
	int16_t reg_aceleracion[3];
	int16_t reg_temperatura;
	float	ft_xa;
	float	ft_ya;
	float	ft_za;
} mpu6050_data_u;

mpu6050_data_u mpu6050;

int MPU6050_Reset(void)
{
	uint8_t out_buf[2];
	int len = 0;
	out_buf[0] = 	_MPU6050_PWR_MGMT_1;
	out_buf[1] = 	_MPU6050_PWR_MGMT_1__DEVICE_RESET__H;
	len = MPU6050_I2C_Escribir( MPU6050_I2C_ADDRESS, out_buf, 2 );

	if(len == 2)
	{
		return 1;
	}
	return -1;
}

int MPU6050_Inicializar(void)
{
	uint8_t out_buf[2];
	int len = 0;

	mpu6050.ft_xa = 0;
	mpu6050.ft_ya = 0;
	mpu6050.ft_xa = 0;

	out_buf[0] =	_MPU6050_PWR_MGMT_1;
	out_buf[1] =	_MPU6050_PWR_MGMT_1__DEVICE_RESET__L |
					_MPU6050_PWR_MGMT_1__SLEEP__L |
					_MPU6050_PWR_MGMT_1__CYCLE__L |
					_MPU6050_PWR_MGMT_1__TEMP_DIS__L;
	len = MPU6050_I2C_Escribir(MPU6050_I2C_ADDRESS, out_buf, 2);

	if(len == 2)
	{
		return 1;
	}
	return -1;
}

int MPU6050_ConfigurarAcelerometro(uint8_t selftest, uint8_t afs_sel)
{
	uint8_t out_buf[2];
	uint8_t afs_sel_value;

	switch (afs_sel) {
		case 1:		afs_sel_value = _MPU6050_ACCEL_CONFIG__AFS_SEL__4g;		break;
		case 2:		afs_sel_value = _MPU6050_ACCEL_CONFIG__AFS_SEL__8g;		break;
		case 3:		afs_sel_value = _MPU6050_ACCEL_CONFIG__AFS_SEL__16g;	break;
		case 0:
		default:	afs_sel_value = _MPU6050_ACCEL_CONFIG__AFS_SEL__2g;		break;
	}

	out_buf[0] =		_MPU6050_ACCEL_CONFIG;
	if(selftest == 0)
	{
		out_buf[1] =	afs_sel_value;
	}
	else
	{
		out_buf[1] =	_MPU6050_ACCEL_CONFIG__XA_ST__H |
						_MPU6050_ACCEL_CONFIG__YA_ST__H |
						_MPU6050_ACCEL_CONFIG__ZA_ST__H |
						afs_sel_value;
	}
	MPU6050_I2C_Escribir( MPU6050_I2C_ADDRESS, out_buf, 2 );

	return 1;
}

int MPU6050__LeerRegistroEscala(void)
{
	int ret = 0;
	uint8_t out_buf[1];
	uint8_t in_buf[1] = {0xaa};
	uint8_t afs_sel;
	int len = 0;

	out_buf[0] = _MPU6050_ACCEL_CONFIG;
	len = MPU6050_I2C_Leer( MPU6050_I2C_ADDRESS, out_buf, 1, in_buf, 1 );

	if(len != 1)
	{
		mpu6050.reg_escala = 1;
		return -1;
	}

	afs_sel = (in_buf[0] >> 3) & 0x03;

	mpu6050.reg_escala = afs_sel;

	ret = 1;
	switch (afs_sel)
	{
		case 0:
			mpu6050.escala = 16 * 1024;
			break;
		case 1:
			mpu6050.escala = 8 * 1024;
			break;
		case 2:
			mpu6050.escala = 4 * 1024;
			break;
		case 3:
			mpu6050.escala = 2 * 1024;
			break;
		default:
			mpu6050.escala = 0;
			ret = -1;
			break;
	}

	return ret;
}

int MPU6050__LeerRegistroAceleracion(void)
{
	int ret = 0;
	uint8_t out_buf[1];
	uint8_t in_buf[6];
	int len = 0;

	out_buf[0] = _MPU6050_ACCEL_XOUT_H;
	len = MPU6050_I2C_Leer( MPU6050_I2C_ADDRESS, out_buf, 1, in_buf, 6 );

	if(len == 6)
	{
		mpu6050.reg_aceleracion[_MPU6050_X] = (in_buf[0] << 8) | in_buf[1];
		mpu6050.reg_aceleracion[_MPU6050_Y] = (in_buf[2] << 8) | in_buf[3];
		mpu6050.reg_aceleracion[_MPU6050_Z] = (in_buf[4] << 8) | in_buf[5];
		return 1;
	}
	mpu6050.reg_aceleracion[_MPU6050_X] = 0;
	mpu6050.reg_aceleracion[_MPU6050_Y] = 0;
	mpu6050.reg_aceleracion[_MPU6050_Z] = 0;
	return -1;
}

int MPU6050__LeerRegistroTemperatura(void)
{
	int ret = 0;
	uint8_t out_buf[1];
	uint8_t in_buf[2];
	int len = 0;

	out_buf[0] = _MPU6050_TEMP_OUT_H;
	len = MPU6050_I2C_Leer( MPU6050_I2C_ADDRESS, out_buf, 1, in_buf, 2 );

	if(len == 2)
	{
		mpu6050.reg_temperatura = (in_buf[0] << 8) | in_buf[1];
		return 1;
	}
	mpu6050.reg_temperatura = 0;
	return -1;
}

int MPU6050__ProcesarLecturas(void)
{
	mpu6050.aceleracion[_MPU6050_X] = ((float)mpu6050.reg_aceleracion[0])/mpu6050.escala;
	mpu6050.aceleracion[_MPU6050_Y] = ((float)mpu6050.reg_aceleracion[1])/mpu6050.escala;
	mpu6050.aceleracion[_MPU6050_Z] = ((float)mpu6050.reg_aceleracion[2])/mpu6050.escala;

	mpu6050.temperatura = ((float)mpu6050.reg_temperatura)/340 + 36.53;

	return 1;
}

int MPU6050_Medir(void)
{
	int ret = 0;

	MPU6050__LeerRegistroEscala();
	MPU6050__LeerRegistroAceleracion();
	MPU6050__ProcesarLecturas();

	return ret;
}

//float MPU6050__CalculoFT(uint8_t test)
//{
//	float ft;
//	if(test != 0)
//	{
//		ft = test * 100;
//	}
//	else
//	{
//		ft = 0.0;
//	}
//
//	return ft;
//}

//int MPU6050__LeerRegistroFT(void)
//{
//	int ret = 0;
//	uint8_t out_buf[4];
//	uint8_t in_buf[1] = {0xaa};
//	uint8_t xa_test, ya_test, za_test;
//	int len;
//
//	out_buf[0] = _MPU6050_SELF_TEST;
//	len = MPU6050_I2C_Leer(MPU6050_I2C_ADDRESS, out_buf, 1, in_buf, 4);
//
//	xa_test = ((in_buf[0] & 0xe0) >> 2) || ((in_buf[0] & 0x30) >> 4);
//	ya_test = ((in_buf[1] & 0xe0) >> 2) || ((in_buf[0] & 0x0c) >> 2);
//	za_test = ((in_buf[3] & 0xe0) >> 2) || ((in_buf[0] & 0x03) >> 0);
//
//	mpu6050.ft_xa = MPU6050__CalculoFT(xa_test);
//	mpu6050.ft_ya = MPU6050__CalculoFT(ya_test);
//	mpu6050.ft_xa = MPU6050__CalculoFT(za_test);
//
//	ret = 1;
//	return ret;
//}

float MPU6050_LeerAceleracionX(void)
{
	return mpu6050.aceleracion[_MPU6050_X];
}

float MPU6050_LeerAceleracionY(void)
{
	return mpu6050.aceleracion[_MPU6050_Y];
}

float MPU6050_LeerAceleracionZ(void)
{
	return mpu6050.aceleracion[_MPU6050_Z];
}

float MPU6050_LeerTemperatura(void)
{
	return mpu6050.temperatura;
}

//int MPU6050_LeerFT(float * ft_xa, float * ft_ya, float * ft_za)
//{
//	*ft_xa = mpu6050.ft_xa;
//	*ft_ya = mpu6050.ft_ya;
//	*ft_za = mpu6050.ft_za;
//	return 1;
//}

void MPU6050_Valor_RegistroAceleracion(int8_t * pValor)
{
	ciaaPOSIX_memcpy(pValor, &mpu6050.reg_aceleracion, sizeof(mpu6050.reg_aceleracion));
}

void MPU6050_Valor_RegistroEscala(int8_t * pValor)
{
	ciaaPOSIX_memcpy(pValor, &mpu6050.reg_escala, sizeof(mpu6050.reg_escala));
}

/** @}*/

/** @}*/
