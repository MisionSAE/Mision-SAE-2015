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
 * bmp085.c
 */

/**
 * @addtogroup Barómetro
 * @{
 */

#include "../inc/bmp085.h"

/**
 * @addtogroup Funciones
 * @{
 */

#define _BMP085__PRUEBA_UT		(27898)
#define _BMP085__PRUEBA_UP		(23843)
#define _BMP085__PRUEBA_AC1		(408)
#define _BMP085__PRUEBA_AC2		(-72)
#define _BMP085__PRUEBA_AC3		(-14383)
#define _BMP085__PRUEBA_AC4		(32741)
#define _BMP085__PRUEBA_AC5		(32757)
#define _BMP085__PRUEBA_AC6		(23153)
#define _BMP085__PRUEBA_B1		(6190)
#define _BMP085__PRUEBA_B2		(4)
#define _BMP085__PRUEBA_MB		(-32768)
#define _BMP085__PRUEBA_MC		(-8711)
#define _BMP085__PRUEBA_MD		(2868)
#define _BMP085__PRUEBA_OSS		(3)

/**
 * @brief	Estructura de abstracción del dispositivo AT24C512
 */
struct bmp085_calibrationCoefficients_s
{
	int16_t 	AC1, AC2, AC3;
	uint16_t 	AC4, AC5, AC6;
	int16_t 	B1, B2, MB, MC, MD;
} __attribute__((packed));

/**
 * @brief	Estructura de abstracción del dispositivo AT24C512
 */
typedef struct bmp085_calibrationCoefficients_s bmp085_calibrationCoefficients_u;

/**
 * @brief	Estructura de abstracción del dispositivo AT24C512
 */
typedef struct
{
	bmp085_calibrationCoefficients_u	coefCal;
	int32_t		UT, UP;
	uint8_t 	oss;
	float 		T;
	float 		P;
} bmp085_data_u;

bmp085_data_u bmp085;

//int BMP085_LeerRegistroTemperatura(void)
//{
//	uint8_t out_buf[2];
//	uint8_t in_buf[2];
//
//	out_buf[0] = _BMP085_REG_READSET;
//	out_buf[1] = _BMP085_REG_READSET__TEMPERATURE;
//	BMP085_I2C_Escribir( BMP085_I2C_ADDRESS, out_buf, 2 );
//
//	BMP085_Esperar(5);
//
//	out_buf[0] = _BMP085_REG_MEASURE__MSB;
//	BMP085_I2C_Leer( BMP085_I2C_ADDRESS, out_buf, 1, in_buf, 2 );
//
//	bmp085.UT = (in_buf[0] << 8) | in_buf[1];
//
//	return 1;
//}

//int BMP085_LeerRegistroPresion(void)
//{
//	uint8_t out_buf[2];
//	uint8_t in_buf[3];
//
//	out_buf[0] = _BMP085_REG_READSET;
//	out_buf[1] = _BMP085_REG_READSET__PRESSURE(bmp085.oss);
//	BMP085_I2C_Escribir( BMP085_I2C_ADDRESS, out_buf, 2 );
//
//	BMP085_Esperar(25);
//
//	out_buf[0] = _BMP085_REG_MEASURE__MSB;
//	BMP085_I2C_Leer( BMP085_I2C_ADDRESS, out_buf, 1, in_buf, 3 );
//
//	bmp085.UP = ((in_buf[0] << 16) | (in_buf[1] << 8) | in_buf[2]) >> (8 - bmp085.oss);
//
//	return 1;
//}

//int BMP085_Medir(void)
//{
//	BMP085_LeerRegistroTemperatura();
//	BMP085_LeerRegistroPresion();
//	BMP085_ProcesarLecturas();
//	return 1;
//}

int BMP085_ProcesarLecturas(void)
{
	int32_t X1, X2, B5, B6;
	int32_t X3, B3;
	uint32_t B4, B7;
	int32_t t, p;

	int32_t aux_int;
	int32_t aux_uint;

	bmp085.T = 0;
	bmp085.P = 0;

	if(bmp085.coefCal.AC1 != 0xffff)
	{
		/*
		 * Calculo de temperatura
		 */
		X1 = ((bmp085.UT - bmp085.coefCal.AC6) * bmp085.coefCal.AC5) >> 15;

		if((X1 + bmp085.coefCal.MD) == 0)
			return -1;

		X2 = (bmp085.coefCal.MC << 11) / (X1 + bmp085.coefCal.MD);
		B5 = X1 + X2;
		t = (B5 + 8) >> 4;
		bmp085.T = ((float)t) / 10;

		/*
		 * Calculo de presión
		 */
		B6 = B5 - 4000;
		X1 = (bmp085.coefCal.B2 * ((B6*B6) >> 12)) >> 11;
		X2 = (bmp085.coefCal.AC2 * B6) >> 11;
		X3 = X1 + X2;
		B3 = (((bmp085.coefCal.AC1 * 4 + X3) << bmp085.oss) + 2) >> 2;
		X1 = (bmp085.coefCal.AC3 * B6) >> 13;
		X2 = (bmp085.coefCal.B1 * ((B6*B6) >> 12)) >> 16;
		X3 = ((X1 + X2) + 2) >> 2;

		B4 = bmp085.coefCal.AC4 * (uint32_t) (X3 + 32768) >> 15;
		B7 = ((uint32_t)bmp085.UP - B3) * (50000 >> bmp085.oss);
		if ( B7 < 0x80000000 )
		{
			if(B4 == 0)
				return -1;

			p = (B7 * 2) / B4;
		}
		else
		{
			p = (B7 / B4) * 2;
		}
		X1 = (p >> 8) * (p >> 8);
		X1 = (X1 * 3038) >> 16;
		X2 = (-7357 * p) >> 16;
		p = p + ((X1 + X2 + 3791) >> 4);
		bmp085.P = p;
		return 1;
	}
	bmp085.T = 0;
	bmp085.P = 0;
	return -1;
}

float BMP085_LeerTemperatura(void)
{
	return bmp085.T;
}

float BMP085_LeerPresion(void)
{
	return bmp085.P;
}

int BMP085_LeerCoeficientes(void)
{
	uint8_t out_buf[1];
	uint8_t in_buf[22];
	int len = 0;

	out_buf[0] = _BMP085_REG_AC1__MSB;
	len = BMP085_I2C_Leer( BMP085_I2C_ADDRESS, out_buf, 1, in_buf, 22 );

	if(len == 22)
	{
		bmp085.coefCal.AC1 	= (in_buf[0] << 8) | in_buf[1];
		bmp085.coefCal.AC2 	= (in_buf[2] << 8) | in_buf[3];
		bmp085.coefCal.AC3 	= (in_buf[4] << 8) | in_buf[5];
		bmp085.coefCal.AC4 	= (in_buf[6] << 8) | in_buf[7];
		bmp085.coefCal.AC5 	= (in_buf[8] << 8) | in_buf[9];
		bmp085.coefCal.AC6 	= (in_buf[10] << 8) | in_buf[11];
		bmp085.coefCal.B1 	= (in_buf[12] << 8) | in_buf[13];
		bmp085.coefCal.B2 	= (in_buf[14] << 8) | in_buf[15];
		bmp085.coefCal.MB 	= (in_buf[16] << 8) | in_buf[17];
		bmp085.coefCal.MC 	= (in_buf[18] << 8) | in_buf[19];
		bmp085.coefCal.MD 	= (in_buf[20] << 8) | in_buf[21];
		return 1;
	}
	ciaaPOSIX_memset(&bmp085.coefCal, 0xff, 22);
	return -1;
}

int BMP085_Prueba_CargarValores()
{
	/*
	 * Según ejemplo del fabricante
	 * Documento: BST-BMP085-DS000-05
	 * Revisión: 1.2
	 */
	bmp085.coefCal.AC1 	= _BMP085__PRUEBA_AC1;
	bmp085.coefCal.AC2 	= _BMP085__PRUEBA_AC2;
	bmp085.coefCal.AC3 	= _BMP085__PRUEBA_AC3;
	bmp085.coefCal.AC4 	= _BMP085__PRUEBA_AC4;
	bmp085.coefCal.AC5 	= _BMP085__PRUEBA_AC5;
	bmp085.coefCal.AC6 	= _BMP085__PRUEBA_AC6;
	bmp085.coefCal.B1 	= _BMP085__PRUEBA_B1;
	bmp085.coefCal.B2 	= _BMP085__PRUEBA_B2;
	bmp085.coefCal.MB 	= _BMP085__PRUEBA_MB;
	bmp085.coefCal.MC 	= _BMP085__PRUEBA_MC;
	bmp085.coefCal.MD 	= _BMP085__PRUEBA_MD;

	bmp085.oss 			= _BMP085__PRUEBA_OSS;
	bmp085.UT 			= _BMP085__PRUEBA_UT;
	bmp085.UP 			= _BMP085__PRUEBA_UP;

	return 1;
}

void BMP085_Valor_RegistroCalibracion(int8_t * pValor)
{
	ciaaPOSIX_memcpy(pValor, &bmp085.coefCal, sizeof(bmp085.coefCal));
}

void BMP085_Valor_RegistroTemperatura(int8_t * pValor)
{
	ciaaPOSIX_memcpy(pValor, &bmp085.UT, sizeof(bmp085.UT));
}

void BMP085_Valor_RegistroPresion(int8_t * pValor)
{
	ciaaPOSIX_memcpy(pValor, &bmp085.UP, sizeof(bmp085.UP));
}

int BMP085_Temperatura_Medir(void)
{
	uint8_t out_buf[2];

	out_buf[0] = _BMP085_REG_READSET;
	out_buf[1] = _BMP085_REG_READSET__TEMPERATURE;
	BMP085_I2C_Escribir( BMP085_I2C_ADDRESS, out_buf, 2 );

	return 1;
}

int BMP085_Temperatura_LeerMedicion(void)
{
	uint8_t out_buf[2];
	uint8_t in_buf[2];
	int len = 0;

	out_buf[0] = _BMP085_REG_MEASURE__MSB;
	len = BMP085_I2C_Leer( BMP085_I2C_ADDRESS, out_buf, 1, in_buf, 2 );

	if(len == 2)
	{
		bmp085.UT = (in_buf[0] << 8) | in_buf[1];
		return 1;
	}
	bmp085.UT = 0;
	return -1;
}
int BMP085_Presion_Medir(void)
{
	uint8_t out_buf[2];

	out_buf[0] = _BMP085_REG_READSET;
	out_buf[1] = _BMP085_REG_READSET__PRESSURE(bmp085.oss);
	BMP085_I2C_Escribir( BMP085_I2C_ADDRESS, out_buf, 2 );

	return 1;
}

int BMP085_Presion_LeerMedicion(void)
{
	uint8_t out_buf[2];
	uint8_t in_buf[3];
	int len = 0;

	out_buf[0] = _BMP085_REG_MEASURE__MSB;
	len = BMP085_I2C_Leer( BMP085_I2C_ADDRESS, out_buf, 1, in_buf, 3 );

	if(len == 3)
	{
		bmp085.UP = ((in_buf[0] << 16) | (in_buf[1] << 8) | in_buf[2]) >> (8 - bmp085.oss);
		return 1;
	}
	bmp085.UP = 0;
	return -1;
}

int BMP085_Inicializar(void)
{
	bmp085.oss	= 3;
	ciaaPOSIX_memset(&bmp085.coefCal, 0xff, 22);

	return BMP085_LeerCoeficientes();
}

/** @}*/

/** @}*/
