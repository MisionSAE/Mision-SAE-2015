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

#include "../inc/app_autodiagnostico.h"

/***********************************************************************
 *
 ***********************************************************************/

static const char	msgModo_AutoDiagnostico[]			= "\r\n\r\n\r\nModo: Auto-Diagnostico.\r\n\r\n";
static const char	msgAutoDiagnostico_Iniciando[]		= "Iniciando Autodiagnostico.\r\n";
static const char	msgAutoDiagnostico_ERROR[]			= "Autodiagnostico finalizado con errores.\r\n";
static const char	msgAutoDiagnostico_OK[]				= "Autodiagnostico finalizado correctamente.\r\n";
static const char	msgBarometro[]						= "Barometro...";
static const char	msgAcelerometro[]					= "Acelerometro...";
static const char	msgMemoria[]						= "Memoria...";
static const char	msgOK[]								= "OK.\r\n";
static const char	msgERROR[]							= "ERROR.\r\n";
static const char	msgTramaAutoDiagnostico[]			= "\r\n###,PRUEBA,%04.0f,%06.0f,%+05.1f,%+08.4f,%+08.4f,%+08.4f";

/***********************************************************************
 *
 ***********************************************************************/

enum
{
	AUTODIAGNOSTICO__ESTADO__TECLA,
	AUTODIAGNOSTICO__ESTADO__INICIO,
	AUTODIAGNOSTICO__ESTADO__BAROMETRO,
	AUTODIAGNOSTICO__ESTADO__ACELEROMETRO,
	AUTODIAGNOSTICO__ESTADO__MEMORIA_ESCRITURA,
	AUTODIAGNOSTICO__ESTADO__MEMORIA_LECTURA,
	AUTODIAGNOSTICO__ESTADO__DATOS,
	AUTODIAGNOSTICO__ESTADO__ERROR,
	AUTODIAGNOSTICO__ESTADO__FIN,
};

/***********************************************************************
 *
 ***********************************************************************/

struct
{
	uint8_t 				estado;
	float					ft_str_x;
	float					ft_str_y;
	float					ft_str_z;
	bool					barometro;
	bool					acelerometro;
	bool					memoria;
} autodiagnostico;

/***********************************************************************
 *
 ***********************************************************************/

static uint32_t		contadorAutoDiagnostico				= 0;

/***********************************************************************
 *
 ***********************************************************************/


int AutoDiagnostico_Inicio(void)
{
	LED_APAGAR(LED_1 | LED_2 | LED_3);

	DEBUG_OUT(msgAutoDiagnostico_Iniciando);
	RADIO_OUT(msgAutoDiagnostico_Iniciando);

	autodiagnostico.barometro = false;
	autodiagnostico.acelerometro = false;
	autodiagnostico.estado = AUTODIAGNOSTICO__ESTADO__BAROMETRO;
	SetRelAlarm(AlarmaAutoDiagnostico, 1000, 0);
	return 1;
}

int AutoDiagnostico_Barometro(void)
{
	memoria_encabezado_u 	encabezado;
	uint16_t				coeficientes[N_COEFICIENTES / 2];
	int 					i;

	LED_APAGAR(LED_1 | LED_2 | LED_3);
	LED_ENCENDER(LED_1);

	DEBUG_OUT(msgBarometro);
	RADIO_OUT(msgBarometro);

	autodiagnostico.barometro = true;
	ciaaPOSIX_memset(coeficientes, 0, N_COEFICIENTES);
	if(BMP085_LeerCoeficientes() != 1)
	{
		autodiagnostico.barometro = false;
	}
	else
	{
		BMP085_Valor_RegistroCalibracion((uint8_t*)&coeficientes);

		for(i = 0; i < (N_COEFICIENTES / 2); i++)
		{
			if((0x0000 == coeficientes[i]) || (coeficientes[i] == 0xffff))
			{
				autodiagnostico.barometro = false;
			}
		}
	}

	if(autodiagnostico.barometro == true)
	{
		DEBUG_OUT(msgOK);
		RADIO_OUT(msgOK);
	}
	else
	{
		DEBUG_OUT(msgERROR);
		RADIO_OUT(msgERROR);
	}

	autodiagnostico.estado = AUTODIAGNOSTICO__ESTADO__ACELEROMETRO;

	SetRelAlarm(AlarmaAutoDiagnostico, 1000, 0);
	return 1;
}

int AutoDiagnostico_Acelerometro()
{
	static int estado = 0;
	float xa, ya, za, res;

	LED_APAGAR(LED_1 | LED_2 | LED_3);
	LED_ENCENDER(LED_2);

	DEBUG_OUT(msgAcelerometro);
	RADIO_OUT(msgAcelerometro);

	autodiagnostico.acelerometro = true;
	if(MPU6050__LeerRegistroAceleracion() != 1)
	{
		autodiagnostico.acelerometro = false;
	}
	else
	{
		MPU6050__ProcesarLecturas();

		xa = MPU6050_LeerAceleracionX();
		ya = MPU6050_LeerAceleracionY();
		za = MPU6050_LeerAceleracionZ();

		res = xa*xa + ya*ya + za*za;

		if(res < (0.8 * 0.8) || (1.2 * 1.2) < res)
		{
			autodiagnostico.acelerometro = false;
		}
	}

	if(autodiagnostico.acelerometro == true)
	{
		DEBUG_OUT(msgOK);
		RADIO_OUT(msgOK);
	}
	else
	{
		DEBUG_OUT(msgERROR);
		RADIO_OUT(msgERROR);
	}

	autodiagnostico.estado = AUTODIAGNOSTICO__ESTADO__MEMORIA_ESCRITURA;

	SetRelAlarm(AlarmaAutoDiagnostico, 1000, 0);
	return 1;
}

int AutoDiagnostico_MemoriaEscritura()
{
	memoria_encabezado_u encabezado;

	LED_APAGAR(LED_1 | LED_2 | LED_3);
	LED_ENCENDER(LED_2);

	DEBUG_OUT(msgMemoria);
	RADIO_OUT(msgMemoria);

	autodiagnostico.memoria = true;
	AT24C512_Direccion(MEMORIA_ENCABEZADO_DIR);
	if(AT24C512_Leer(&encabezado, MEMORIA_ENCABEZADO_LON) != 1)
	{
		autodiagnostico.memoria = false;
	}
	else
	{
		encabezado.reservados[1] = ~MEMORIA_ENCABEZADO_RESERVADO;
		AT24C512_Direccion(MEMORIA_ENCABEZADO_DIR);
		if(AT24C512_Grabar(&encabezado, MEMORIA_ENCABEZADO_LON) != 1)
		{
			autodiagnostico.memoria = false;
		}
	}

	autodiagnostico.estado = AUTODIAGNOSTICO__ESTADO__MEMORIA_LECTURA;

	SetRelAlarm(AlarmaAutoDiagnostico, 1000, 0);
	return 1;
}

int AutoDiagnostico_MemoriaLectura()
{
	memoria_encabezado_u encabezado;

	AT24C512_Direccion(MEMORIA_ENCABEZADO_DIR);
	if(AT24C512_Leer(&encabezado, MEMORIA_ENCABEZADO_LON) != 1)
	{
		autodiagnostico.memoria = false;
	}
	else
	{
		if(encabezado.reservados[1] != (uint8_t)(~MEMORIA_ENCABEZADO_RESERVADO))
		{
			autodiagnostico.memoria = false;
		}
	}

	encabezado.reservados[1] = MEMORIA_ENCABEZADO_RESERVADO;
	AT24C512_Direccion(MEMORIA_ENCABEZADO_DIR);
	if(AT24C512_Grabar(&encabezado, MEMORIA_ENCABEZADO_LON) != 1)
	{
		autodiagnostico.memoria = false;
	}

	if(autodiagnostico.memoria == true)
	{
		DEBUG_OUT(msgOK);
		RADIO_OUT(msgOK);
	}
	else
	{
		DEBUG_OUT(msgERROR);
		RADIO_OUT(msgERROR);
	}

	if(		autodiagnostico.barometro 		== true &&
			autodiagnostico.acelerometro 	== true &&
			autodiagnostico.memoria 		== true )
	{
		autodiagnostico.estado = AUTODIAGNOSTICO__ESTADO__DATOS;
		DEBUG_OUT(msgAutoDiagnostico_OK);
		RADIO_OUT(msgAutoDiagnostico_OK);
	}
	else
	{
		autodiagnostico.estado = AUTODIAGNOSTICO__ESTADO__ERROR;
	}

	SetRelAlarm(AlarmaAutoDiagnostico, 1000, 0);
	return 1;
}

int AutoDiagnostico_Datos()
{
	static char			buffer[128];
	static uint32_t		nromedicion = 0;
	VariablesMision		medicionDiagnostico;
	int					ret;

	LED_APAGAR(LED_1 | LED_2 | LED_3);
	LED_ENCENDER(LED_3);
	autodiagnostico.estado = AUTODIAGNOSTICO__ESTADO__DATOS;

	ret = Medicion();

	if(ret == MEDICION_OK)
	{
		medicionDiagnostico.nroMedicion					= nromedicion;
		medicionDiagnostico.presion_Pascal				= medicion.dato.presion;
		medicionDiagnostico.temperatura_Centigrados 	= medicion.dato.temperatura;
		medicionDiagnostico.aceleracion_x				= medicion.dato.acel[0];
		medicionDiagnostico.aceleracion_y				= medicion.dato.acel[1];
		medicionDiagnostico.aceleracion_z				= medicion.dato.acel[2];

		ArmarTrama(buffer, msgTramaAutoDiagnostico, &medicionDiagnostico);
		DEBUG_OUT(buffer);
		RADIO_OUT(buffer);

		nromedicion++;

		SetRelAlarm(AlarmaAutoDiagnostico, 1000, 0);
	}

	return 1;
}

int AutoDiagnostico_ERROR()
{
	LED_APAGAR(LED_1 | LED_2 | LED_3);
	LED_ENCENDER(LED_3);

	DEBUG_OUT(msgAutoDiagnostico_ERROR);
	RADIO_OUT(msgAutoDiagnostico_ERROR);

	while(1);

	return 1;
}

int AutoDiagnostico(void)
{
	static int contadorIniciado = 0;
	int ret = 0;

	if(contadorIniciado == 0)
	{
		autodiagnostico.estado = AUTODIAGNOSTICO__ESTADO__INICIO;
	}

	switch (autodiagnostico.estado)
	{
	case AUTODIAGNOSTICO__ESTADO__INICIO: 				AutoDiagnostico_Inicio(); 				break;
	case AUTODIAGNOSTICO__ESTADO__BAROMETRO: 			AutoDiagnostico_Barometro(); 			break;
	case AUTODIAGNOSTICO__ESTADO__ACELEROMETRO: 		AutoDiagnostico_Acelerometro(); 		break;
	case AUTODIAGNOSTICO__ESTADO__MEMORIA_ESCRITURA: 	AutoDiagnostico_MemoriaEscritura(); 	break;
	case AUTODIAGNOSTICO__ESTADO__MEMORIA_LECTURA: 		AutoDiagnostico_MemoriaLectura(); 		break;
	case AUTODIAGNOSTICO__ESTADO__DATOS: 				AutoDiagnostico_Datos(); 				break;
	case AUTODIAGNOSTICO__ESTADO__ERROR: 				AutoDiagnostico_ERROR(); 				break;
	}

	contadorIniciado++;

	return ret;
}

/***********************************************************************
 *
 ***********************************************************************/

TASK(TareaAutoDiagnostico)
{
	static bool			inicializado = false;

	if(!inicializado)
	{
		inicializado = true;
		LED_APAGAR(LED_TODOS);

		DEBUG_OUT(msgModo_AutoDiagnostico);
		RADIO_OUT(msgModo_AutoDiagnostico);
	}

	if(sae.estado_dispositivo == SAE_DISPOSITIVO_INICIALIZADO)
	{
		LED_ENCENDER(LEDCOLOR_AZUL);
		contadorAutoDiagnostico++;
		AutoDiagnostico();
	}
	else
	{
		LED_PARPADEAR(LEDCOLOR_AZUL);
		SetRelAlarm(AlarmaAutoDiagnostico, 250, 0);
	}

	TerminateTask();
}
