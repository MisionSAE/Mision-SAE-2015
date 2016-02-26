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


#include "../inc/app_recuperacion.h"

/***********************************************************************
 *
 ***********************************************************************/

static const char	msgModo_Recuperacion[]		= "\r\n\r\n\r\nModo: Recuperación de datos\r\n\r\n";
static const char	msgLecturaPresionar[]		= "Presionar una tecla\r\n";
static const char	msgEncabezadoERROR[]		= "ERROR en lectura del Encabezado\r\n";
static const char	msgLeyendoEncabezado[]		= "Lectura del Encabezado\r\n";
static const char	msgTramaCRC_OK[]			= "\r\n+++,PRUEBA,%04.0f,%06.0f,%+05.1f,%+08.4f,%+08.4f,%+08.4f";
static const char	msgTramaCRC_ERROR[]			= "\r\n---,PRUEBA,%04.0f,%06.0f,%+05.1f,%+08.4f,%+08.4f,%+08.4f";
static const char	msgRecuperandoDatos[]		= "Recuperando datos\r\n";
static const char	msgRecuperacionFin[]		= "\r\nRecuperacion Fin";

/***********************************************************************
 *
 ***********************************************************************/

enum
{
	RECUPERACION__ESTADO__TECLA,
	RECUPERACION__ESTADO__INICIO,
	RECUPERACION__ESTADO__ENCABEZADO,
	RECUPERACION__ESTADO__DATOS,
	RECUPERACION__ESTADO__LEER_MEMORIA,
	RECUPERACION__ESTADO__FIN,
};

/***********************************************************************
 *
 ***********************************************************************/

struct
{
	uint8_t 				estado;
	uint16_t				bloque;
	memoria_encabezado_u 	encabezado;
} recuperacion;

/***********************************************************************
 *
 ***********************************************************************/

static uint32_t		contadorRecuperacion				= 0;

/***********************************************************************
 *
 ***********************************************************************/

int Recuperacion_Tecla(void)
{
	static int 	estado 		= 0;
	static int 	contador 	= 0;
	uint8_t 	teclas;
	int 		ret 		= 0;

	switch (estado)
	{
		case 0:
			LED_APAGAR(LED_1 | LED_2 | LED_3);
			RADIO_OUT(msgLecturaPresionar);
			DEBUG_OUT(msgLecturaPresionar);
			contador = 0;
			estado++;
			break;
		case 1:
			ciaaPOSIX_read(teclado, &teclas, 1);
			teclas &= 0x0f;
			LED_PARPADEAR(LED_3);
			if(teclas)
			{
				estado++;
			}
			break;
		case 2:
			LED_APAGAR(LED_1 | LED_2 | LED_3);
			recuperacion.estado = RECUPERACION__ESTADO__INICIO;
			break;
	}

	SetRelAlarm(AlarmaRecuperacionDatos, 100, 0);
	return ret;
}

int Recuperacion_Inicio(void)
{
	LED_APAGAR(LED_1 | LED_2 | LED_3);
	LED_ENCENDER(LED_1);
	recuperacion.estado = RECUPERACION__ESTADO__ENCABEZADO;
	SetRelAlarm(AlarmaRecuperacionDatos, 1000, 0);
	return 1;
}

int Recuperacion_Encabezado(void)
{
	memoria_encabezado_u encabezado;
	AT24C512_Direccion(MEMORIA_ENCABEZADO_DIR);
	AT24C512_Leer(&recuperacion.encabezado, MEMORIA_ENCABEZADO_LON);

	if(recuperacion.encabezado.marca != MEMORIA_ENCABEZADO_MARCA)
	{
		RADIO_OUT(msgEncabezadoERROR);
		DEBUG_OUT(msgEncabezadoERROR);
	}
	else
	{
		RADIO_OUT(msgLeyendoEncabezado);
		DEBUG_OUT(msgLeyendoEncabezado);
	}

	recuperacion.bloque = 0;
	recuperacion.estado = RECUPERACION__ESTADO__DATOS;
	SetRelAlarm(AlarmaRecuperacionDatos, 50, 0);
	return 1;
}

int Recuperacion_Datos()
{
	static char		buffer[128];
	memoria_dato_u 	binario;
	VariablesMision promedio;
	uint8_t 		crc;

	if(recuperacion.bloque == 0)
	{
		RADIO_OUT(msgRecuperandoDatos);
		DEBUG_OUT(msgRecuperandoDatos);
	}

	AT24C512_Direccion(MEMORIA_DATO_DIR + MEMORIA_DATO_LON * recuperacion.bloque);
	AT24C512_Leer((uint8_t*)&binario, MEMORIA_DATO_LON);

	if(binario.marca != MEMORIA_DATO_MARCA)
	{
		recuperacion.estado = RECUPERACION__ESTADO__FIN;
	}
	else
	{
		Binario_a_Decimal(&promedio, &binario);
		crc = CalcularCRC(&binario, MEMORIA_DATO_LON - 2);
		if(binario.crc == crc)
		{
			ArmarTrama(buffer, msgTramaCRC_OK, &promedio);
		}
		else
		{
			ArmarTrama(buffer, msgTramaCRC_ERROR, &promedio);
		}
		XBeeTransmitirDatos(buffer, ciaaPOSIX_strlen(buffer));
		DEBUG_OUT(buffer);

		recuperacion.bloque++;
		if(recuperacion.bloque >= ((MEMORIA_CAPACIDAD - 32) / 16))
		{
			recuperacion.estado = RECUPERACION__ESTADO__FIN;
		}
	}
	SetRelAlarm(AlarmaRecuperacionDatos, 50, 0);
	return 1;
}

int Recuperacion_Fin(void)
{
	LED_APAGAR(LED_1 | LED_2 | LED_3);
	LED_ENCENDER(LED_2);
	RADIO_OUT(msgRecuperacionFin);
	DEBUG_OUT(msgRecuperacionFin);
	while(1);
	return 1;
}

int Recuperacion(void)
{
	static int contadorIniciado = 0;
	int ret = 0;

	if(contadorIniciado == 0)
	{
		recuperacion.estado = RECUPERACION__ESTADO__TECLA;
	}

	switch (recuperacion.estado)
	{
	case RECUPERACION__ESTADO__TECLA: 		Recuperacion_Tecla(); 		break;
	case RECUPERACION__ESTADO__INICIO: 		Recuperacion_Inicio(); 		break;
	case RECUPERACION__ESTADO__ENCABEZADO: 	Recuperacion_Encabezado(); 	break;
	case RECUPERACION__ESTADO__DATOS: 		Recuperacion_Datos(); 		break;
	case RECUPERACION__ESTADO__FIN:			Recuperacion_Fin(); 		break;
	}

	contadorIniciado++;

	return ret;
}

/***********************************************************************
 *
 ***********************************************************************/

TASK(TareaRecuperacionDatos)
{
	static bool			inicializado = false;

	if(!inicializado)
	{
		inicializado = true;
		LED_APAGAR(LED_TODOS);

		DEBUG_OUT(msgModo_Recuperacion);
		RADIO_OUT(msgModo_Recuperacion);
	}

	if(sae.estado_dispositivo == SAE_DISPOSITIVO_INICIALIZADO)
	{
		LED_ENCENDER(LEDCOLOR_BLANCO);
		contadorRecuperacion++;
		Recuperacion();
	}
	else
	{
		LED_PARPADEAR(LEDCOLOR_BLANCO);
		SetRelAlarm(AlarmaRecuperacionDatos, 250, 0);
	}

	TerminateTask();
}
