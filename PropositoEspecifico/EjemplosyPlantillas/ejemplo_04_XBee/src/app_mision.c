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


#include "../inc/app_mision.h"

/***********************************************************************
 * Textos utilizado
 ***********************************************************************/

static const char	msgMisionSAE[] = "\r\n*** Misión SAE ***\r\nInicio!\r\n";

/***********************************************************************
 * Variables globales
 ***********************************************************************/

uint8_t leds; // estado de los leds de la placa
// valores de sensores
float valor_aceleracion_x;
float valor_presion;
float valor_temperatura;
/***********************************************************************
 * Tareas
 ***********************************************************************/

/** \brief Mision primaria
 * En esta Tarea se deben formatear lso datos de los sensores y enviarlos por
 * radio a la estación terrena
 */
TASK(TareaMisionPrimaria)
{
	static bool inicializado = false;
	static bool sae_inicializado = false;
	uint8_t teclas;
	static int tiempo = 0;
	char mensaje[100];

	if(!inicializado)
	{
		inicializado = true;
		/*
		 * La tarea se activará en Ti=920ms y se activará cada Tc=250ms
		 * No es recomendable utilizar un valor Tc inferior para "TareaMisionPrimaria"
		 * No es recomendable un valor de Ti igual para todas las tareas
		 */
		SetRelAlarm(AlarmaMisionPrimaria, 920, 250);
	}

	if(sae.estado_dispositivo == SAE_DISPOSITIVO_INICIALIZADO)
	{
/**********************************************************************
 * Inicio del código para la misión primaria ->
 **********************************************************************/

		if(!sae_inicializado)
		{
			// Mensaje de comienzo de misión
			sae_inicializado = true;
			DEBUG_OUT(msgMisionSAE);
			RADIO_OUT(msgMisionSAE);
		}

		/*
		 * Se utiliza el contador "tiempo" para enviar una flag a la
		 * tarea de sensores 1 vez por segundo
		 * tiempo cuenta de 0 a 3
		 *
		 * 4 * 250ms = 1000ms = 1s
		 */
		tiempo++;
		if(tiempo >= 3)
		{
			tiempo = 0;
			/*
			 * Dar formato a las mediciones y enviarlas
			 */
			sprintf_float(mensaje, "INICIO,presion=%.2f,", valor_presion);
			XBeeTransmitirDatos(mensaje, ciaaPOSIX_strlen(mensaje));

			sprintf_float(mensaje, "temperatura=%.5f,", valor_temperatura);
			XBeeTransmitirDatos(mensaje, ciaaPOSIX_strlen(mensaje));

			sprintf_float(mensaje, "aceleracionx=%.5f,FIN\r\n", valor_aceleracion_x);
			XBeeTransmitirDatos(mensaje, ciaaPOSIX_strlen(mensaje));
		}

		/*
		 * Boton de parada de proceso
		 * Apretando la tecla correspondiente, el proceso se detiene.
		 * Esto provoca que el sistema operativo reporte un error y que la tarea no se ejecute
		 * Las tareas de mayor prioridad se ejecutan normalmente
		 */
		do
		{
			ciaaPOSIX_read(teclado, &teclas, 1);
		}
		while( teclas & TECLA_2 );

		/*
		 * Led indicador de ejecución
		 * Realiza un cambio de estado cada vez que se ejecuta la tarea
		 */
		leds ^= LED_1;
		ciaaPOSIX_write(display, &leds, 1);

/**********************************************************************
 * <- Fin del código de la misión primaria
 **********************************************************************/
	}
	else
	{
		/*
		 * Mientras se configuran los sensores se realiza un
		 * parpadeo del LED RGB
		 */
		leds ^= LED_0_R;
		leds ^= LED_0_G;
		leds ^= LED_0_B;
		ciaaPOSIX_write(display, &leds, 1);
	}

	ciaaPOSIX_write(display, &leds, 1);

	TerminateTask();
}

/** \brief Mision secundaria
 *
 */
TASK(TareaMisionSecundaria)
{
	static bool inicializado = false;
	uint8_t teclas;

	if(!inicializado)
	{
		inicializado = true;
		/*
		 * La tarea se activará en Ti=940ms y se activará cada Tc=1000ms
		 * No es recomendable utilizar un valor Tc inferior para "TareaMisionSecundaria"
		 * No es recomendable un valor de Ti igual para todas las tareas
		 */
		SetRelAlarm(AlarmaMisionSecundaria, 940, 1000);
	}

	if(sae.estado_dispositivo == SAE_DISPOSITIVO_INICIALIZADO)
	{
/**********************************************************************
 * Inicio del código para la misión secundaria ->
 **********************************************************************/

		/*
		 * Boton de parada de proceso
		 * Apretando la tecla correspondiente, el proceso se detiene.
		 * Esto provoca que el sistema operativo reporte un error y que la tarea no se ejecute
		 * Las tareas de mayor prioridad se ejecutan normalmente
		 */
		do
		{
			ciaaPOSIX_read(teclado, &teclas, 1);
		}
		while( teclas & TECLA_3 );

		/*
		 * Led indicador de ejecución
		 * Realiza un cambio de estado cada vez que se ejecuta la tarea
		 */
		leds ^= LED_2;
		ciaaPOSIX_write(display, &leds, 1);

/**********************************************************************
 * <- Fin del código de la misión secundaria
 **********************************************************************/
	}

	TerminateTask();
}

/*
 * Tarea LeerSensores:
 * En esta tarea se leen los sensores de la sonda
 * Se recomiendo almacenar los valores de aceleración, temperatura y presión
 * en variable globales para ser accedidas por el resto de las tareas
 */
TASK(TareaLeerSensores)
{
	static bool inicializado = false;
	uint8_t teclas;

	if(!inicializado)
	{
		inicializado = true;
		/*
		 * La tarea se activará en Ti=900ms y se activará cada Tc=100ms
		 * No es recomendable utilizar un valor Tc inferior para "TareaLeerSensores"
		 * No es recomendable un valor de Ti igual para todas las tareas
		 */
		SetRelAlarm(AlarmaCicloSensores, 900, 100);
	}

	if(sae.estado_dispositivo == SAE_DISPOSITIVO_INICIALIZADO)
	{
/**********************************************************************
 * Inicio del código para leer sensores ->
 **********************************************************************/

		valor_presion 		= 101800;
		valor_temperatura 	= 25.3;
		valor_aceleracion_x = 9.8;

		/*
		 * Boton de parada de proceso
		 * Apretando la tecla correspondiente, el proceso se detiene.
		 * Esto provoca que el sistema operativo reporte un error y que la tarea no se ejecute
		 * Las tareas de mayor prioridad se ejecutan normalmente
		 */
		do
		{
			ciaaPOSIX_read(teclado, &teclas, 1);
		}
		while( teclas & TECLA_4 );

		/*
		 * Led indicador de ejecución
		 * Realiza un cambio de estado cada vez que se ejecuta la tarea
		 */
		leds ^= LED_3;
		ciaaPOSIX_write(display, &leds, 1);

/**********************************************************************
 * <- Fin del código para leer sensores
 **********************************************************************/
	}

	TerminateTask();
}
