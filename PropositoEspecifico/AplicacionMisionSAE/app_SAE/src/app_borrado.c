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


#include "../inc/app_borrado.h"

/***********************************************************************
 *
 ***********************************************************************/

static const char	msgModo_Borrado[]			= "\r\n\r\n\r\nModo: Borrado de datos\r\n\r\n";
static const char	msgLeyendo[]				= "Verificando memoria\r\n";
static const char	msgBorrando[]				= "Borrando memoria\r\n";
static const char	msgMemoriaBorrada[]			= "Memoria borrada\r\n";
static const char	msgMemoriaError[]			= "Error al borrar memoria\r\n";
static const char	msgMemoriaFin[]				= "\r\nFin del proceso\r\n";
static const char	msgMemoriaPresionar2[]		= "Presionar tecla 2\r\n";
static const char	msgMemoriaPresionar3[]		= "Presionar tecla 3\r\n";
static const char	msgMemoriaPresionar4[]		= "Presionar tecla 4\r\n";
static const char	msgMemoriaSoltar2[]			= "Soltar tecla 2\r\n";
static const char	msgMemoriaSoltar3[]			= "Soltar tecla 3\r\n";
static const char	msgMemoriaSoltar4[]			= "Soltar tecla 4\r\n";
static const char	msgBorrandoPagina[]			= "Borrando página: %1.0f\r\n";
static const char	msgVerificandoPagina[]		= "Verificando página: %1.0f\r\n";

/***********************************************************************
 *
 ***********************************************************************/

enum
{
	BORRADO__ESTADO__INICIO,
	BORRADO__ESTADO__TECLA2,
	BORRADO__ESTADO__TECLA3,
	BORRADO__ESTADO__TECLA4,
	BORRADO__ESTADO__BORRAR_MEMORIA,
	BORRADO__ESTADO__VERIFICAR,
	BORRADO__ESTADO__OK,
	BORRADO__ESTADO__ERROR,
	BORRADO__ESTADO__FIN,
};

/***********************************************************************
 *
 ***********************************************************************/

struct
{
	uint8_t 			estado;
	int					pagina;
} borrado;

/***********************************************************************
 *
 ***********************************************************************/

static uint32_t		contadorBorrado			= 0;

/***********************************************************************
 *
 ***********************************************************************/

int Borrado_Tecla(uint8_t led, uint8_t tecla, const char * msgPresionar, const char * msgSoltar, int estado_falso, int estado_verdadero)
{
	static int 	estado 		= 0;
	static int 	contador 	= 0;
	uint8_t 	teclas;
	int 		ret 		= 0;

	switch (estado)
	{
		case 0:
			LED_APAGAR(LED_1 | LED_2 | LED_3);
			RADIO_OUT(msgPresionar);
			DEBUG_OUT(msgPresionar);
			contador = 0;
			estado++;
			break;
		case 1:
			ciaaPOSIX_read(teclado, &teclas, 1);
			teclas &= 0x0f;
			if((teclas & tecla) && !(teclas & ~tecla))
			{
				LED_PARPADEAR(led);
				contador++;
				if(contador >= 10)
				{
					estado++;
					RADIO_OUT(msgSoltar);
					DEBUG_OUT(msgSoltar);
				}
			}
			else if(teclas & ~tecla)
			{
				ret = -1;
				estado = 0;
			}
			else
			{
				LED_ENCENDER(led);
				contador = 0;
			}
			break;
		case 2:
			LED_APAGAR(LED_1 | LED_2 | LED_3);
			ciaaPOSIX_read(teclado, &teclas, 1);
			teclas &= 0x0f;
			if(!teclas)
			{
				estado++;
			}
			else if(teclas & ~tecla)
			{
				ret = -1;
				estado = 0;
			}
			break;
		case 3:
			ret = 1;
			estado = 0;
			break;
		default:
			ret = -1;
			estado = 0;
			break;
	}

	switch(ret)
	{
		case -1: borrado.estado = estado_falso; break;
		case  1: borrado.estado = estado_verdadero; break;
	}

	SetRelAlarm(AlarmaBorrardoDatos, 100, 0);
	return ret;
}

int Borrado_Inicio(void)
{
	LED_APAGAR(LED_1 | LED_2 | LED_3);
	borrado.pagina = 0;
	borrado.estado = BORRADO__ESTADO__BORRAR_MEMORIA;
	SetRelAlarm(AlarmaBorrardoDatos, 1000, 0);
	return 1;
}

int Borrado_BorrarMemoria(void)
{
	static int 	contador = 0;
	char 		message[20];
	uint8_t 	buffer[MEMORIA_PAGINA_CAPACIDAD];

	if(borrado.pagina == 0)
	{
		LED_APAGAR(LED_1 | LED_2 | LED_3);
		LED_ENCENDER(LED_2);
		RADIO_OUT(msgBorrando);
		DEBUG_OUT(msgBorrando);
		AT24C512_Direccion(0x0000);
	}

	contador++;
	if(contador >= (500/50))
	{
		contador = 0;
		LED_PARPADEAR(LED_2);
	}

	ciaaPOSIX_memset(buffer, MEMORIA_RESERVADO, MEMORIA_PAGINA_CAPACIDAD);
	if(AT24C512_Grabar(buffer, MEMORIA_PAGINA_CAPACIDAD) != 1)
	{
		borrado.estado = BORRADO__ESTADO__ERROR;
		SetRelAlarm(AlarmaBorrardoDatos, 1000, 0);
	}
	else
	{
		sprintf_float(message, msgBorrandoPagina, borrado.pagina);
		RADIO_OUT(message);
		DEBUG_OUT(message);

		borrado.pagina++;
		if(borrado.pagina >= MEMORIA_PAGINA_CANTIDAD)
		{
			LED_APAGAR(LED_1 | LED_2 | LED_3);
			borrado.pagina = 0;
			borrado.estado = BORRADO__ESTADO__VERIFICAR;
			SetRelAlarm(AlarmaBorrardoDatos, 1000, 0);
		}
		else
		{
			SetRelAlarm(AlarmaBorrardoDatos, 30, 0);
		}
	}
	return 1;
}

int Borrado_Verificar(void)
{
	static int contador = 0;
	int			i;
	char 		message[20];
	uint8_t 	buffer[MEMORIA_PAGINA_CAPACIDAD];

	if(borrado.pagina == 0)
	{
		LED_APAGAR(LED_1 | LED_2 | LED_3);
		LED_ENCENDER(LED_1);
		RADIO_OUT(msgLeyendo);
		DEBUG_OUT(msgLeyendo);
	}

	contador++;
	if(contador >= (500/50))
	{
		contador = 0;
		LED_PARPADEAR(LED_1);
	}

	ciaaPOSIX_memset(buffer, ~MEMORIA_RESERVADO, MEMORIA_PAGINA_CAPACIDAD);
	AT24C512_Direccion(MEMORIA_PAGINA_CAPACIDAD * borrado.pagina);
	if(AT24C512_Leer(buffer, MEMORIA_PAGINA_CAPACIDAD) != 1)
	{
		borrado.estado = BORRADO__ESTADO__ERROR;
		SetRelAlarm(AlarmaBorrardoDatos, 1000, 0);
	}
	else
	{
		sprintf_float(message, msgVerificandoPagina, borrado.pagina);
		RADIO_OUT(message);
		DEBUG_OUT(message);

		for(i = 0; (i < MEMORIA_PAGINA_CAPACIDAD) && (borrado.estado != BORRADO__ESTADO__ERROR); i++)
		{
			if(buffer[i] != MEMORIA_RESERVADO)
			{
				borrado.estado = BORRADO__ESTADO__ERROR;
			}
		}

		if(borrado.estado == BORRADO__ESTADO__ERROR)
		{
			SetRelAlarm(AlarmaBorrardoDatos, 1000, 0);
		}
		else
		{
			borrado.pagina++;
			if(borrado.pagina >= MEMORIA_PAGINA_CANTIDAD)
			{
				borrado.estado = BORRADO__ESTADO__OK;

				SetRelAlarm(AlarmaBorrardoDatos, 1000, 0);
			}
			else
			{
				SetRelAlarm(AlarmaBorrardoDatos, 30, 0);
			}
		}
	}
	return 1;
}

int Borrado_Error(void)
{
	LED_APAGAR(LED_1 | LED_2 | LED_3);
	LED_ENCENDER(LED_2);
	RADIO_OUT(msgMemoriaError);
	DEBUG_OUT(msgMemoriaError);
	borrado.estado = BORRADO__ESTADO__FIN;
	SetRelAlarm(AlarmaBorrardoDatos, 1000, 0);
	return 1;
}

int Borrado_OK(void)
{
	LED_APAGAR(LED_1 | LED_2 | LED_3);
	LED_ENCENDER(LED_3);
	RADIO_OUT(msgMemoriaBorrada);
	DEBUG_OUT(msgMemoriaBorrada);
	borrado.estado = BORRADO__ESTADO__FIN;
	SetRelAlarm(AlarmaBorrardoDatos, 1000, 0);
	return 1;
}

int Borrado_Fin(void)
{
	RADIO_OUT(msgMemoriaFin);
	DEBUG_OUT(msgMemoriaFin);
	while(1);
	return 1;
}

int Borrado(void)
{
	static int contadorIniciado = 0;
	int ret = 0;

	if(contadorIniciado == 0)
	{
		borrado.estado = BORRADO__ESTADO__TECLA2;
	}

	switch (borrado.estado)
	{
	case BORRADO__ESTADO__TECLA2:			Borrado_Tecla(LED_1, TECLA_2, msgMemoriaPresionar2, msgMemoriaSoltar2, BORRADO__ESTADO__TECLA2, BORRADO__ESTADO__TECLA3);	break;
	case BORRADO__ESTADO__TECLA3:			Borrado_Tecla(LED_2, TECLA_3, msgMemoriaPresionar3, msgMemoriaSoltar3, BORRADO__ESTADO__TECLA2, BORRADO__ESTADO__TECLA4);	break;
	case BORRADO__ESTADO__TECLA4:			Borrado_Tecla(LED_3, TECLA_4, msgMemoriaPresionar4, msgMemoriaSoltar4, BORRADO__ESTADO__TECLA2, BORRADO__ESTADO__INICIO);	break;
	case BORRADO__ESTADO__INICIO:			Borrado_Inicio();			break;
	case BORRADO__ESTADO__BORRAR_MEMORIA:	Borrado_BorrarMemoria();	break;
	case BORRADO__ESTADO__VERIFICAR:		Borrado_Verificar();		break;
	case BORRADO__ESTADO__OK:				Borrado_OK();				break;
	case BORRADO__ESTADO__ERROR:			Borrado_Error();			break;
	case BORRADO__ESTADO__FIN:				Borrado_Fin();				break;
	}

	contadorIniciado++;

	return ret;
}

/***********************************************************************
 *
 ***********************************************************************/

/*
 * Tarea BorrarMemoria:
 *
 * Se ejecuta una vez.
 * Formatea la memoria EEPROM.
 * Grava todos los bytes con el valor 0xFF
 */
TASK(TareaBorradoDatos)
{
	static bool			inicializado 			= false;

	if(!inicializado)
	{
		inicializado = true;
		LED_APAGAR(LED_TODOS);

		DEBUG_OUT(msgModo_Borrado);
		RADIO_OUT(msgModo_Borrado);
	}

	if(sae.estado_dispositivo == SAE_DISPOSITIVO_INICIALIZADO)
	{
		LED_ENCENDER(LEDCOLOR_ROJO);
		contadorBorrado++;
		Borrado();
	}
	else
	{
		LED_PARPADEAR(LEDCOLOR_ROJO);
		SetRelAlarm(AlarmaBorrardoDatos, 250, 0);
	}

	TerminateTask();
}
