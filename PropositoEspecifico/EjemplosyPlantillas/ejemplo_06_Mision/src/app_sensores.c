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


#include "../inc/app_sensores.h"

/***********************************************************************
 *
 ***********************************************************************/

sensores_u sensores;

/***********************************************************************
 *
 ***********************************************************************/

#define MEDICION_TIEMPO_INICIO									(5)
#define MEDICION_TIEMPO_TEMPERATURA								(15)
#define MEDICION_TIEMPO_PRESION									(40)

/***********************************************************************
 *
 ***********************************************************************/

enum
{
	SENSORES__ESTADO__ACELEROMETRO_RESET,
	SENSORES__ESTADO__ACELEROMETRO_CONFIGURAR,
	SENSORES__ESTADO__BAROMETRO_CONFIGURAR,
	SENSORES__ESTADO__FIN,
};

/***********************************************************************
 *
 ***********************************************************************/

int Sensores_AcelerometroReset(int * pTiempo)
{
	sae.estado_acelerometro = SAE_DISPOSITIVO_ACELEROMETRO_OK;
	if(MPU6050_Inicializar() != SAE_OK)
	{
		sae.estado_acelerometro = SAE_DISPOSITIVO_ACELEROMETRO_ERROR;
	}
	else
	{
		if(MPU6050_ConfigurarAcelerometro(0, 3) != SAE_OK)
		{
			sae.estado_acelerometro = SAE_DISPOSITIVO_ACELEROMETRO_ERROR;
		}
	}

	sensores.estado = SENSORES__ESTADO__ACELEROMETRO_CONFIGURAR;

	(*pTiempo) = 100;

	return 1;
}

int Sensores_AcelerometroConfigurar(int * pTiempo)
{
	if(MPU6050__LeerRegistroEscala() != SAE_OK)
	{
		sae.estado_acelerometro = SAE_DISPOSITIVO_ACELEROMETRO_ERROR;
	}
	MPU6050_Valor_RegistroEscala(&sensores.escala);

	sae.estado_barometro = SAE_DISPOSITIVO_BAROMETRO_OK;
	if(BMP085_Inicializar() != SAE_OK)
	{
		sae.estado_barometro = SAE_DISPOSITIVO_BAROMETRO_ERROR;
	}

	sensores.estado = SENSORES__ESTADO__BAROMETRO_CONFIGURAR;

	(*pTiempo) = 100;

	return 1;
}

int Sensores_BarometroConfigurar(int * pTiempo)
{
	BMP085_Valor_RegistroCalibracion(&sensores.coeficientes);
	sensores.estado = SENSORES__ESTADO__FIN;

	(*pTiempo) = 100;

	return 1;
}

int Sensores_Inicializar(int * pTiempo)
{
	static uint contadorIniciado = 0;
	int ret = SENSORES_INICIALIZANDO;

	if(contadorIniciado == 0)
	{
		sensores.estado = SENSORES__ESTADO__ACELEROMETRO_RESET;
		contadorIniciado++;
	}

	switch (sensores.estado)
	{
	case SENSORES__ESTADO__ACELEROMETRO_RESET:		Sensores_AcelerometroReset(pTiempo);		break;
	case SENSORES__ESTADO__ACELEROMETRO_CONFIGURAR:	Sensores_AcelerometroConfigurar(pTiempo);	break;
	case SENSORES__ESTADO__BAROMETRO_CONFIGURAR:	Sensores_BarometroConfigurar(pTiempo);		break;
	case SENSORES__ESTADO__FIN:
			(*pTiempo) = 100;
			sensores.estado = SENSORES__ESTADO__FIN;
			ret = SENSORES_OK;
			break;
	}

	return ret;
}
