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
 *
 ***********************************************************************/

static const char	msgXBee_Iniciando[]				= "Iniciando XBee...\r\n";
static const char	msgXBee_Iniciado[]				= "XBee iniciado.\r\n";
static const char	msgSensores_Iniciando[]			= "Iniciando Sensores...\r\n";
static const char	msgSensores_Iniciado[]			= "Sensores iniciados.\r\n";
static const char	msgMemoriaDisponible[]			= "Hay %04.0f datos en memoria.\r\n";
static const char	msgMemoriaAvisoBorrar[]			= "Debe proceder a borrar los datos de la memoria.\r\n";
static const char	msgSensores_Acelerometro[]		= "Acelerometro...";
static const char	msgSensores_Barometro[]			= "Barometro...";
static const char	msgSensores_Memoria[]			= "Memoria...";
static const char	msgSensores_OK[]				= "OK.\r\n";
static const char	msgSensores_ERROR[]				= "ERRROR.\r\n";

/***********************************************************************
 *
 ***********************************************************************/

sae_u 		sae;

/***********************************************************************
 *
 ***********************************************************************/

/** \brief Main function
 *
 */
int main(void)
{
	/*
	 * CIAA			LPC4337		GPIO	DIP SWITCH
	 * LCD1			P4.4		2.4
	 * LCD2			P4.5		2.5
	 * LCD3			P4.6		2.6
	 * LCD_RS		P4.8		5.12
	 */
	Chip_SCU_PinMuxSet(4, 4, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));
	Chip_SCU_PinMuxSet(4, 5, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));
	Chip_SCU_PinMuxSet(4, 6, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0));
	Chip_SCU_PinMuxSet(4, 8, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4));

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 4, 4);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 4, 5);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 4, 6);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 4, 8);

	sae.modo = 0;
	sae.modo |= (Chip_GPIO_GetPinState(LPC_GPIO_PORT, 2, 4)  ? 1 : 0) << 0;
	sae.modo |= (Chip_GPIO_GetPinState(LPC_GPIO_PORT, 2, 5)  ? 1 : 0) << 1;
	sae.modo |= (Chip_GPIO_GetPinState(LPC_GPIO_PORT, 2, 6)  ? 1 : 0) << 2;
	sae.modo |= (Chip_GPIO_GetPinState(LPC_GPIO_PORT, 5, 12) ? 1 : 0) << 3;

	switch(sae.modo)
	{
		case MODO_BORRADO_VALOR:			StartOS(AppModeBorradoDatos);		break;
		case MODO_AUTODIAGNOSTICO_VALOR:	StartOS(AppModeAutoDiagnostico);	break;
		case MODO_RECUPERACION_VALOR:		StartOS(AppModeRecuperacionDatos);	break;
		case MODO_MISION_VALOR:
		default:							StartOS(AppModeMision);				break;
	}

	return 0;
}

/***********************************************************************
 *
 ***********************************************************************/

/** \brief Tarea Init
 *
 * Esta tarea es autoarrancada por el sistema en el modo de aplicación 1.
 * Se hace la inicialización y apertura de dispositivos y se activa la tarea encargada de arrancar el XBee.
 *
 */
TASK(TareaInicializacion)
{
	ciaak_start();

	teclado			= ciaaPOSIX_open("/dev/dio/in/0", O_RDONLY);
	display			= ciaaPOSIX_open("/dev/dio/out/0", O_RDWR);
	uartUSB			= ciaaPOSIX_open("/dev/serial/uart/1", O_RDWR | O_NONBLOCK);
	uartExpansion	= ciaaPOSIX_open("/dev/serial/uart/2", O_RDWR | O_NONBLOCK);
	i2cExpansion 	= ciaaPOSIX_open("/dev/i2c/0", O_RDWR);

	/*
	 * Info sacada del esquemático de la EDU-CIAA-NXP: DISPOSITIVO a PIN
	 * Info sacada de la hoja de datos del micro: PIN a USARTx
	 * Info sacada de ciaaDriverUart.c: PIN a DEVICE
	uart/0	->	USART0	-> 485
	uart/1	->	USART2	-> u2
	uart/2	->	USART3	-> expansion
	 */
	/* Configurar los RS232 */
	ciaaPOSIX_ioctl(uartUSB,		ciaaPOSIX_IOCTL_SET_BAUDRATE,			(void *)ciaaBAUDRATE_57600);
	ciaaPOSIX_ioctl(uartUSB,		ciaaPOSIX_IOCTL_SET_FIFO_TRIGGER_LEVEL,	(void *)ciaaFIFO_TRIGGER_LEVEL3);
	ciaaPOSIX_ioctl(uartExpansion,	ciaaPOSIX_IOCTL_SET_BAUDRATE,			(void *)ciaaBAUDRATE_57600);
	ciaaPOSIX_ioctl(uartExpansion,	ciaaPOSIX_IOCTL_SET_FIFO_TRIGGER_LEVEL,	(void *)ciaaFIFO_TRIGGER_LEVEL3);

	sae.estado_dispositivo = SAE_DISPOSITIVO_NO_INICIALIZADO;
	LED_ENCENDER(LED_TODOS);

	XBeeIniciar(uartExpansion);

	ChainTask(TareaInicializarXBee);
}

/** \brief Inicialización del XBee
 *
 * Se llama esta tarea una vez por segundo.
 * Una vez que está funcionando el XBee, recién ahí puedo activar las otras tareas.
 *
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
//				XBeeSaveConfig				();
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


int TareaInicializarSensores_Inicio(int * pContador)
{
	DEBUG_OUT(msgSensores_Iniciando);
	RADIO_OUT(msgSensores_Iniciando);
	sae.tiempo = 100;
	(*pContador)++;
	return 1;
}

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

int TareaInicializarSensores_Informe(int * pContador)
{
																DEBUG_OUT(msgSensores_Barometro);	RADIO_OUT(msgSensores_Barometro);
	if(sae.estado_barometro == SAE_DISPOSITIVO_BAROMETRO_OK)		{ DEBUG_OUT(msgSensores_OK); 		RADIO_OUT(msgSensores_OK); }
	else															{ DEBUG_OUT(msgSensores_ERROR); 	RADIO_OUT(msgSensores_ERROR); }
																DEBUG_OUT(msgSensores_Acelerometro);	RADIO_OUT(msgSensores_Acelerometro);
	if(sae.estado_acelerometro == SAE_DISPOSITIVO_ACELEROMETRO_OK)	{ DEBUG_OUT(msgSensores_OK);		RADIO_OUT(msgSensores_OK); }
	else															{ DEBUG_OUT(msgSensores_ERROR); 	RADIO_OUT(msgSensores_ERROR); }
																DEBUG_OUT(msgSensores_Memoria);			RADIO_OUT(msgSensores_Memoria);
	if(sae.estado_memoria == SAE_DISPOSITIVO_MEMORIA_OK)			{ DEBUG_OUT(msgSensores_OK);		RADIO_OUT(msgSensores_OK); }
	else															{ DEBUG_OUT(msgSensores_ERROR); 	RADIO_OUT(msgSensores_ERROR); }

	(*pContador)++;
	sae.tiempo = 100;

	return 1;
}

int TareaInicializarSensores_Fin(int * pContador)
{
	static char 	buffer[128];
	int 			n_datos;

	if(sae.estado_memoria == SAE_DISPOSITIVO_MEMORIA_OK)
	{
		if(sae.estado_memoria_disponible != SAE_DISPOSITIVO_MEMORIA_VACIA)
		{
			if(sae.estado_memoria_disponible == SAE_DISPOSITIVO_MEMORIA_CARGADA)
			{
				n_datos = (sensores.direccion - MEMORIA_DATO_DIR) / MEMORIA_DATO_LON;
			}
			else
			{
				n_datos = (MEMORIA_CAPACIDAD - MEMORIA_DATO_DIR) / MEMORIA_DATO_LON;
				sae.estado_memoria_disponible = SAE_DISPOSITIVO_MEMORIA_LLENA;
			}
			sprintf_float(buffer, msgMemoriaDisponible, n_datos);
			DEBUG_OUT(buffer);
			RADIO_OUT(buffer);
		}
	}

	sae.estado_dispositivo = SAE_DISPOSITIVO_INICIALIZADO;
	DEBUG_OUT(msgSensores_Iniciado);
	RADIO_OUT(msgSensores_Iniciado);

	(*pContador)++;

	sae.tiempo = 100;

	return 1;
}

/** \brief Inicialización de sensores
 *
 * Se llama esta tarea una vez por segundo.
 * Una vez que está funcionando el XBee, recién ahí puedo activar las otras tareas.
 *
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

/***********************************************************************
 *
 ***********************************************************************/

/** \brief Error Hook function
 *
 */
void ErrorHook(void)
{
	static char 		buffer[128];

//	ciaaPOSIX_printf("ErrorHook was called\n");
//	ciaaPOSIX_printf("Service: %d, P1: %d, P2: %d, P3: %d, RET: %d\n", OSErrorGetServiceId(), OSErrorGetParam1(), OSErrorGetParam2(), OSErrorGetParam3(), OSErrorGetRet());

	sprintf_float(buffer, "\r\n\r\nService: %4.0f\r\n", OSErrorGetServiceId());
	DEBUG_OUT(buffer);
	RADIO_OUT(buffer);

	sprintf_float(buffer, "P1: %4.0f\r\n", OSErrorGetParam1());
	DEBUG_OUT(buffer);
	RADIO_OUT(buffer);

	sprintf_float(buffer, "P2: %4.0f\r\n", OSErrorGetParam2());
	DEBUG_OUT(buffer);
	RADIO_OUT(buffer);

	sprintf_float(buffer, "P3: %4.0f\r\n", OSErrorGetParam3());
	DEBUG_OUT(buffer);
	RADIO_OUT(buffer);

	sprintf_float(buffer, "RET: %4.0f\r\n", OSErrorGetRet());
	DEBUG_OUT(buffer);
	RADIO_OUT(buffer);

	ShutdownOS(0);
}
