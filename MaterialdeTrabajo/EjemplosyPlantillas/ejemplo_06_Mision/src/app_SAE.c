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


#include "../inc/app_SAE.h"

/***********************************************************************
 * Textos utilizado
 ***********************************************************************/

static const char	msgXBee_Iniciando[]				= "Iniciando XBee...\r\n";
static const char	msgXBee_Iniciado[]				= "XBee iniciado.\r\n";
static const char	msgSensores_Iniciando[]			= "Iniciando Sensores...\r\n";
static const char	msgSensores_Iniciado[]			= "Sensores iniciados.\r\n";
static const char	msgSensores_Acelerometro[]		= "Acelerometro...";
static const char	msgSensores_Barometro[]			= "Barometro...";
static const char	msgSensores_OK[]				= "OK.\r\n";
static const char	msgSensores_ERROR[]				= "ERRROR.\r\n";

/***********************************************************************
 * Variables globales
 ***********************************************************************/

sae_u sae;

/***********************************************************************
 * Funciones
 ***********************************************************************/

/** \brief Main function
 * Inicio del sistema
 * Inicio del sistema operativo
 */
int main(void)
{
	StartOS(AppModeMision);
	return 0;
}

/** \brief Tarea Init
 * Es la primer tarea que se ejecuta.
 * Inicia CIAA-Firmware y sus dispositivos
 * Realiza una mínima configuración de la UART
 * Al finalizar se ejecuta la tarea "TareaInicializarXBee"
 */
TASK(TareaInicializacion)
{
	ciaak_start();

	teclado			= ciaaPOSIX_open("/dev/dio/in/0", O_RDONLY);
	display			= ciaaPOSIX_open("/dev/dio/out/0", O_RDWR);
	uartUSB			= ciaaPOSIX_open("/dev/serial/uart/1", O_RDWR | O_NONBLOCK);
	uartExpansion	= ciaaPOSIX_open("/dev/serial/uart/2", O_RDWR | O_NONBLOCK);
	i2cExpansion 	= ciaaPOSIX_open("/dev/i2c/0", O_RDWR);

	/* Configurar los RS232 */
	ciaaPOSIX_ioctl(uartUSB,		ciaaPOSIX_IOCTL_SET_BAUDRATE,			(void *)ciaaBAUDRATE_57600);
	ciaaPOSIX_ioctl(uartUSB,		ciaaPOSIX_IOCTL_SET_FIFO_TRIGGER_LEVEL,	(void *)ciaaFIFO_TRIGGER_LEVEL3);
	ciaaPOSIX_ioctl(uartExpansion,	ciaaPOSIX_IOCTL_SET_BAUDRATE,			(void *)ciaaBAUDRATE_57600);
	ciaaPOSIX_ioctl(uartExpansion,	ciaaPOSIX_IOCTL_SET_FIFO_TRIGGER_LEVEL,	(void *)ciaaFIFO_TRIGGER_LEVEL3);

	sae.estado_dispositivo = SAE_DISPOSITIVO_NO_INICIALIZADO;

	XBeeIniciar(uartExpansion);

	ChainTask(TareaInicializarXBee);
}

/** \brief Inicialización del XBee
 * Configura el XBee para la conexión con la estación terrena
 * Es necesario que previamente se configure el XBee con un baudrate de 57600
 * Al finalizar se ejecuta la tarea "TareaInicializarSensores"
 */
TASK(TareaInicializarXBee)
{
	static bool			inicializado			= false;
	static int			contadorInicializacion	= 0;

	// Inicializamos el XBee si no está inicializado
	if(!inicializado)
	{
		// Informamos que estamos inicializando el XBee
		if(contadorInicializacion == 1)
		{
			DEBUG_OUT(msgXBee_Iniciando);
		}

		GetResource(ResourceUART);
		{
			// Si pudimos entrar en modo comandos
			if(XBeeEntrarModoComandos(&contadorInicializacion))
			{
				// Enviar comandos
				XBeeSetChannel				(CANAL);
				XBeeSetBaudRate				(XBEE_BAUD_57600);
				XBeeSetOwnAddress			(DIRECCION_COHETE);
				XBeeSetPANID				(DIRECCION_RED);
				XBeeSetDestinationAddress	(DIRECCION_TERRENA);
				XBeeTransmitirDatos			("ATPL4\x0d", 6);
#if 0
				XBeeSaveConfig				(); // no recomendado
#endif
				XBeeSalirModoComandos		();
				XBeeTransmitirDatos			("\x0d\x0a", 2);
				inicializado = true;
			}
		}
		ReleaseResource(ResourceUART);

		if(inicializado)
		{
			DEBUG_OUT(msgXBee_Iniciado);
			ChainTask(TareaInicializarSensores);
		}
		else
		{
			SetRelAlarm(AlarmaInicializarXBee, 1100, 0);
		}
	}

	TerminateTask();
}

/*
 * Estado del proceso inicializar sensores.
 * Ejecutado en "TareaInicializarSensores"
 */
int TareaInicializarSensores_Inicio(int * pContador)
{
	UARTS_OUT(msgSensores_Iniciando);
	sae.tiempo = 100;
	(*pContador)++;
	return 1;
}

/*
 * Estado del proceso inicializar sensores.
 * Ejecutado en "TareaInicializarSensores"
 */
int TareaInicializarSensores_Sensores(int * pContador)
{
	int ret;

	GetResource(ResourceBusI2C);
	{
		ret = Sensores_Inicializar(&sae.tiempo);
	}
	ReleaseResource(ResourceBusI2C);

	switch(ret)
	{
	case SENSORES_INICIALIZANDO:							break;
	case SENSORES_OK:				(*pContador)++;			break;
	case SENSORES_ERROR:
	default:
		while(1);
		break;
	}
	return 1;
}

/*
 * Estado del proceso inicializar sensores.
 * Ejecutado en "TareaInicializarSensores"
 */
int TareaInicializarSensores_Informe(int * pContador)
{
	UARTS_OUT(msgSensores_Barometro);

	if(sae.estado_barometro == SAE_DISPOSITIVO_BAROMETRO_OK)
	{
		UARTS_OUT(msgSensores_OK);
	}
	else
	{
		UARTS_OUT(msgSensores_ERROR);
	}

	UARTS_OUT(msgSensores_Acelerometro);

	if(sae.estado_acelerometro == SAE_DISPOSITIVO_ACELEROMETRO_OK)
	{
		UARTS_OUT(msgSensores_OK);
	}
	else
	{
		UARTS_OUT(msgSensores_ERROR);
	}

	(*pContador)++;
	sae.tiempo = 100;

	return 1;
}

/*
 * Estado del proceso inicializar sensores.
 * Ejecutado en "TareaInicializarSensores"
 */
int TareaInicializarSensores_Fin(int * pContador)
{
	sae.estado_dispositivo = SAE_DISPOSITIVO_INICIALIZADO;

	UARTS_OUT(msgSensores_Iniciado);

	(*pContador)++;

	sae.tiempo = 100;

	return 1;
}

/** \brief Inicialización de sensores
 * Esta tarea es la encargada de inicilizar el acelerómetro y el barómetro
 * Se comporta como una máquina de estados, comenzando por el estado 0 y
 * finalizando en el estado 4
 * El tiempo de activación es variable, definido en la variable "sae.tiempo"
 * Cuando finaliza la tarea, la variable sae.estado_dispositivo permite la
 * ejecución de las tareas correspondiente a misión primaria, secundaria y
 * de lectura de sensores (ver el archivo app_mision.c)
 */
TASK(TareaInicializarSensores)
{
	static bool			inicializado 			= false;
	static int			contadorInicializacion 	= 0;

	sae.tiempo = 0;
	if(!inicializado)
	{
		switch (contadorInicializacion)
		{
		case 0: TareaInicializarSensores_Inicio(&contadorInicializacion);	break;
		case 1: TareaInicializarSensores_Sensores(&contadorInicializacion); break;
		case 2: TareaInicializarSensores_Informe(&contadorInicializacion); 	break;
		case 3: TareaInicializarSensores_Fin(&contadorInicializacion); 		break;
		case 4: inicializado = true; 										break;
		default:
			while(1);
			break;
		}
	}

	if(sae.tiempo > 0)
	{
		SetRelAlarm(AlarmaInicializarSensores, sae.tiempo, 0);
	}

	TerminateTask();
}

/** \brief Error Hook function
 * Es la función que ejecuta el sistema operativo cuando ocurre un error
 * La información del error se repota por la UART USB
 */
void ErrorHook(void)
{
	static char buffer[128];

	sprintf_float(buffer, "\r\nService:%.0f,", OSErrorGetServiceId());
	DEBUG_OUT(buffer);

	sprintf_float(buffer, "P1:%.0f,", OSErrorGetParam1());
	DEBUG_OUT(buffer);

	sprintf_float(buffer, "P2:%.0f,", OSErrorGetParam2());
	DEBUG_OUT(buffer);

	sprintf_float(buffer, "P3:%.0f,", OSErrorGetParam3());
	DEBUG_OUT(buffer);

	sprintf_float(buffer, "RET:%.0f\r\n", OSErrorGetRet());
	DEBUG_OUT(buffer);

#if 0
	/*
	 * Esta instrucción detiene la ejecución
	 */
	ShutdownOS(0);
#endif
}
