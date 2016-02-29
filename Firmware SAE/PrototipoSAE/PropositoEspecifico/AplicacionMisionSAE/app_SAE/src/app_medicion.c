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


#include "../inc/app_medicion.h"

/***********************************************************************
 *
 ***********************************************************************/

medicion_u medicion;

/***********************************************************************
 *
 ***********************************************************************/

#define MEDICION_TIEMPO_TEMPERATURA								(15)
#define MEDICION_TIEMPO_PRESION									(40)

/***********************************************************************
 *
 ***********************************************************************/

enum
{
	MEDICION__INICIO,
	MEDICION__MEDIR_ACELERACION,
	MEDICION__MEDIR_BAROMETRO,
	MEDICION__MEDIR_TEMPERATURA,
	MEDICION__MEDIR_PRESION,
	MEDICION__FIN,
};

/***********************************************************************
 *
 ***********************************************************************/

int Medicion_MedirAceleracion(void)
{
	if(MPU6050__LeerRegistroAceleracion() != 1)
	{
		medicion.estado_acelerometro = false;
	}
	else
	{
		if(MPU6050__LeerRegistroTemperatura() != 1)
		{
			medicion.estado_acelerometro = false;
		}
	}

	if(medicion.estado_acelerometro == true)
	{
		MPU6050__ProcesarLecturas();
		medicion.dato.acel[0] = MPU6050_LeerAceleracionX();
		medicion.dato.acel[1] = MPU6050_LeerAceleracionY();
		medicion.dato.acel[2] = MPU6050_LeerAceleracionZ();
		medicion.dato.temp 	  = MPU6050_LeerTemperatura();
	}
	else
	{
		medicion.dato.acel[0] = 0;
		medicion.dato.acel[1] = 0;
		medicion.dato.acel[2] = 0;
		medicion.dato.temp 	  = 0;
	}

	if(medicion.estado_barometro == false)
	{
		medicion.estado = MEDICION__FIN;
	}
	else
	{
		medicion.estado = MEDICION__MEDIR_BAROMETRO;

		if(sae.modo == MODO_AUTODIAGNOSTICO_VALOR)
			SetRelAlarm(AlarmaAutoDiagnostico, 5, 0);
		else
			SetRelAlarm(AlarmaLeerSensores, 5, 0);
	}
	return 1;
}

int Medicion_MedirBarometro(void)
{
	medicion.estado = MEDICION__MEDIR_TEMPERATURA;

	BMP085_Temperatura_Medir();

	if(sae.modo == MODO_AUTODIAGNOSTICO_VALOR)
		SetRelAlarm(AlarmaAutoDiagnostico, MEDICION_TIEMPO_TEMPERATURA, 0);
	else
		SetRelAlarm(AlarmaLeerSensores, MEDICION_TIEMPO_TEMPERATURA, 0);
	return 1;
}

int Medicion_MedirTemperatura(void)
{
	if(BMP085_Temperatura_LeerMedicion() != 1)
	{
		medicion.estado = MEDICION__FIN;
		medicion.estado_barometro = false;
		medicion.dato.temperatura = 0;
		medicion.dato.presion = 0;
	}
	else
	{
		medicion.estado = MEDICION__MEDIR_PRESION;
		BMP085_Presion_Medir();

		if(sae.modo == MODO_AUTODIAGNOSTICO_VALOR)
			SetRelAlarm(AlarmaAutoDiagnostico, MEDICION_TIEMPO_PRESION, 0);
		else
			SetRelAlarm(AlarmaLeerSensores, MEDICION_TIEMPO_PRESION, 0);
	}
	return 1;
}

int Medicion_MedirPresion(void)
{
	if(BMP085_Presion_LeerMedicion() != 1)
	{
		medicion.estado = MEDICION__FIN;
		medicion.estado_barometro = false;
		medicion.dato.temperatura = 0;
		medicion.dato.presion = 0;
	}
	else
	{
		BMP085_ProcesarLecturas();
		medicion.dato.temperatura 	= BMP085_LeerTemperatura();
		medicion.dato.presion		= BMP085_LeerPresion();
		medicion.estado 			= MEDICION__FIN;
	}

	return 1;
}

int Medicion(void)
{
	static uint contadorIniciado = 0;
	int ret = MEDICION_PROCESANDO;

	if(contadorIniciado == 0)
	{
		medicion.estado_acelerometro = false;
		if(sae.estado_acelerometro == SAE_DISPOSITIVO_ACELEROMETRO_OK)
		{
			medicion.estado_acelerometro = true;
		}

		medicion.estado_barometro = false;
		if(sae.estado_barometro == SAE_DISPOSITIVO_BAROMETRO_OK)
		{
			medicion.estado_barometro = true;
		}

		if(medicion.estado_acelerometro == true)
		{
			medicion.estado = MEDICION__MEDIR_ACELERACION;
		}
		else
		{
			medicion.dato.acel[0] = 0;
			medicion.dato.acel[1] = 0;
			medicion.dato.acel[2] = 0;
			medicion.dato.temp 	  = 0;

			if(medicion.estado_barometro == true)
			{
				medicion.estado = MEDICION__MEDIR_BAROMETRO;
			}
			else
			{
				medicion.dato.temperatura = 0;
				medicion.dato.presion = 0;
				medicion.estado = MEDICION__FIN;
			}
		}
	}

	switch (medicion.estado)
	{
	case MEDICION__MEDIR_ACELERACION:	Medicion_MedirAceleracion();	break;
	case MEDICION__MEDIR_BAROMETRO:		Medicion_MedirBarometro();		break;
	case MEDICION__MEDIR_TEMPERATURA:	Medicion_MedirTemperatura();	break;
	case MEDICION__MEDIR_PRESION:		Medicion_MedirPresion();		break;
	}

	if(medicion.estado == MEDICION__FIN)
	{
		if(medicion.estado_acelerometro == true)
		{
			medicion.estado = MEDICION__MEDIR_ACELERACION;
		}
		else
		{
			medicion.dato.acel[0] = 0;
			medicion.dato.acel[1] = 0;
			medicion.dato.acel[2] = 0;
			medicion.dato.temp 	  = 0;

			if(medicion.estado_barometro == true)
			{
				medicion.estado = MEDICION__MEDIR_BAROMETRO;
			}
			else
			{
				medicion.dato.temperatura = 0;
				medicion.dato.presion = 0;
				medicion.estado = MEDICION__FIN;
			}
		}
		ret = MEDICION_OK;
	}

	if(ret == MEDICION_PROCESANDO)
		contadorIniciado++;
	else
		contadorIniciado = 1;

	return ret;
}
