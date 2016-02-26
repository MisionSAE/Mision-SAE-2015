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

//#define INICILIZARSENSORES_TIEMPO_RESET_ACELEROMETRO			(5)
//#define INICILIZARSENSORES_TIEMPO_CONFIGURACION_ACELEROMETRO	(5)
//#define INICILIZARSENSORES_TIEMPO_CONFIGURACION_BAROMETRO		(5)
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
	SENSORES__ESTADO__MEMORIA_ENCABEZADO,
	SENSORES__ESTADO__MEMORIA_DATOS,
	SENSORES__ESTADO__ESPACIO_MEMORIA,
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

	if(MPU6050_ConfigurarAcelerometro(0,_MPU6050_ACCEL_CONFIG__AFS_SEL__16g) != SAE_OK)
	{
		sae.estado_acelerometro = SAE_DISPOSITIVO_ACELEROMETRO_ERROR;
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

	AT24C512_Inicializar();

	sensores.estado = SENSORES__ESTADO__MEMORIA_ENCABEZADO;

	(*pTiempo) = 100;

	return 1;
}

int Sensores_MemoriaEncabezado(int * pTiempo)
{
	memoria_encabezado_u encabezado;

	sae.estado_memoria = SAE_DISPOSITIVO_MEMORIA_OK;

	AT24C512_Direccion(MEMORIA_ENCABEZADO_DIR);

	if(AT24C512_Leer(&encabezado, MEMORIA_ENCABEZADO_LON) != SAE_OK)
	{
		sae.estado_memoria = SAE_DISPOSITIVO_MEMORIA_ERROR;
	}
	else
	{
		if(encabezado.marca != MEMORIA_ENCABEZADO_MARCA)
		{
			ciaaPOSIX_memset(&encabezado, MEMORIA_ENCABEZADO_RESERVADO, MEMORIA_ENCABEZADO_LON);
			encabezado.marca 		= MEMORIA_ENCABEZADO_MARCA;
			ciaaPOSIX_memcpy(&encabezado.coeficientes, &sensores.coeficientes, N_COEFICIENTES);
			encabezado.contador 	= 1;
			encabezado.crc 			= CalcularCRC(&encabezado, MEMORIA_ENCABEZADO_LON - 1);
		}
		else
		{
			encabezado.contador++;
			encabezado.crc 			= CalcularCRC(&encabezado, MEMORIA_ENCABEZADO_LON - 1);
		}

		AT24C512_Direccion(MEMORIA_ENCABEZADO_DIR);

		if(AT24C512_Grabar(&encabezado, MEMORIA_ENCABEZADO_LON) != SAE_OK)
		{
			sae.estado_memoria = SAE_DISPOSITIVO_MEMORIA_ERROR;
		}
	}

	sensores.direccion = MEMORIA_DATO_DIR;

	sensores.estado = SENSORES__ESTADO__MEMORIA_DATOS;

	sae.estado_memoria_disponible = SAE_DISPOSITIVO_MEMORIA_VACIA;

	(*pTiempo) = 100;

	return 1;
}

int Sensores_MemoriaDatos(int * pTiempo)
{
	memoria_dato_u dato;

	AT24C512_Direccion(sensores.direccion);

	if(AT24C512_Leer((uint8_t*)&dato, MEMORIA_DATO_LON) != SAE_OK)
	{
		sae.estado_memoria = SAE_DISPOSITIVO_MEMORIA_ERROR;
		sae.estado_memoria_disponible = SAE_DISPOSITIVO_MEMORIA_LLENA;
		sensores.estado = SENSORES__ESTADO__FIN;
	}
	else
	{
		if(dato.marca == MEMORIA_DATO_MARCA)
		{
			sensores.direccion += MEMORIA_DATO_LON;
			sae.estado_memoria_disponible = SAE_DISPOSITIVO_MEMORIA_CARGADA;
		}
		else
		{
			sensores.estado = SENSORES__ESTADO__FIN;
		}

		if(sensores.direccion < MEMORIA_DATO_DIR)
		{
			sae.estado_memoria_disponible = SAE_DISPOSITIVO_MEMORIA_LLENA;
			sensores.direccion = MEMORIA_DATO_DIR;
			sensores.estado = SENSORES__ESTADO__FIN;
		}
	}

	(*pTiempo) = 10;

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
	case SENSORES__ESTADO__MEMORIA_ENCABEZADO:		Sensores_MemoriaEncabezado(pTiempo);		break;
	case SENSORES__ESTADO__MEMORIA_DATOS:			Sensores_MemoriaDatos(pTiempo);			break;
	case SENSORES__ESTADO__FIN:
			//TODO: Validar si fallo algún sensor, dar error o dar ok
			(*pTiempo) = 100;
			sensores.estado = SENSORES__ESTADO__FIN;
			ret = SENSORES_OK;
			break;
	}

	return ret;
}
