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
 *
 ***********************************************************************/

static const char	msgModo_Mision[]			= "\r\n\r\n\r\nModo: Misión\r\n\r\n";
static const char	msgPrimaria_Inicio[]		= "Ejecutando mision primaria\r\n";
static const char	msgPrimaria_Fin[]			= "Mision primaria terminada\r\n";
static const char	msgSecundaria_Inicio[]		= "Ejecutando mision secundaria\r\n";
static const char	msgSecundaria_Fin[]			= "Mision secundaria terminada\r\n";
static const char	msgSensores_Inicio[]		= "Ejecutando sensores\r\n";
static const char	msgSensores_Fin[]			= "Sensores terminada\r\n";
static const char	msgSensores_Medicion[]		= "Medicion %04.0f\r\n";
static const char	msgTramaMision[]			= "\r\n***,PRUEBA,%04.0f,%06.0f,%+05.1f,%+08.4f,%+08.4f,%+08.4f";
static const char	msgTramaSecundaria[]		= ",%+05.1f";

/***********************************************************************
 *
 ***********************************************************************/

static uint32_t			contadorLeerSensores		= 0;
static uint32_t			contadorMisionPrimaria		= 0;
static uint32_t			contadorMisionSecundaria	= 0;
static float			temperaturaSecundaria		= 0;
static VariablesMision 	variablesMision[Nmax_MEDICIONES_xCICLO];
static uint32_t			nroMedicionMision 			= 0;
static uint32_t			nroMedicionActual			= 0;
static uint8_t			vivoMisionPrimaria			= 0;

/***********************************************************************
 *
 ***********************************************************************/

void VariablesMision_CalcularPromedio(VariablesMision * pPromedio)
{
	int i;

	pPromedio->presion_Pascal 				= 0;
	pPromedio->temperatura_Centigrados 		= 0;
	pPromedio->aceleracion_x 				= 0;
	pPromedio->aceleracion_y 				= 0;
	pPromedio->aceleracion_z 				= 0;

	for(i = 0; i < Nmax_MEDICIONES_xCICLO; i++)
	{
		pPromedio->presion_Pascal 			+= variablesMision[i].presion_Pascal;
		pPromedio->temperatura_Centigrados 	+= variablesMision[i].temperatura_Centigrados;
		pPromedio->aceleracion_x 			+= variablesMision[i].aceleracion_x;
		pPromedio->aceleracion_y 			+= variablesMision[i].aceleracion_y;
		pPromedio->aceleracion_z 			+= variablesMision[i].aceleracion_z;
	}

	pPromedio->presion_Pascal 				= pPromedio->presion_Pascal				/ Nmax_MEDICIONES_xCICLO;
	pPromedio->temperatura_Centigrados 		= pPromedio->temperatura_Centigrados 	/ Nmax_MEDICIONES_xCICLO;
	pPromedio->aceleracion_x 				= pPromedio->aceleracion_x 				/ Nmax_MEDICIONES_xCICLO;
	pPromedio->aceleracion_y 				= pPromedio->aceleracion_y 				/ Nmax_MEDICIONES_xCICLO;
	pPromedio->aceleracion_z 				= pPromedio->aceleracion_z 				/ Nmax_MEDICIONES_xCICLO;
}

/***********************************************************************
 *
 ***********************************************************************/

/** \brief Mision primaria
 *
 * 	Esta funcion se ejecuta por primera vez luego de 1 segundo de iniciar la aplicacion en modo vuelo.
 * 	Esta demora obedece a que se tomen 10 mediciones de los sensores antes de la primera ejecución.
 * 	En cada ejecucion tiene 10 mediciones crudas de todos los sensores.
 * 	Puede tomar una cualquiera u obtener el promedio de las 10 o cualquier calculo que desee sobre las mismas.
 * 	En este ejemplo toma siempre la primera de las 10.
 *
 * 	Luego se ejecuta periodicamente cada 1 segundo.
 *
 */
TASK(TareaMisionPrimaria)
{
	static bool				inicializado 			= false;
	static char				buffer[Nmax_TRANSMISIONES_xTRAMA][128];
	static char				bufferSecundario[128];
	static uint8_t			ntx = 0;
	VariablesMision			promedio;
	memoria_dato_u			binario;
	uint16_t				memdir;
	uint8_t 				teclas;

//	ciaaPOSIX_read(teclado, &teclas, 1);
//	if(!(teclas & TECLA_3))
//	{
		vivoMisionPrimaria = 1;
//	}

	if(!inicializado)
	{
		inicializado = true;

		ciaaPOSIX_memset(buffer[0], '\0', 128);
		ciaaPOSIX_memset(buffer[1], '\0', 128);
		ciaaPOSIX_memset(buffer[2], '\0', 128);
		ciaaPOSIX_memset(buffer[3], '\0', 128);
		temperaturaSecundaria = -1;

		LED_APAGAR(LED_TODOS);

		GetResource(ResourceUART);
		{
			DEBUG_OUT(msgModo_Mision);
			RADIO_OUT(msgModo_Mision);
		}
		ReleaseResource(ResourceUART);



		SetRelAlarm(AlarmaMisionPrimaria, 1000/Nmax_TRANSMISIONES_xTRAMA, 1000/Nmax_TRANSMISIONES_xTRAMA);
	}

	if(sae.estado_dispositivo == SAE_DISPOSITIVO_INICIALIZADO && nroMedicionMision > 10)
	{
		DEBUG_OUT(msgPrimaria_Inicio);
		LED_ENCENDER(LEDCOLOR_VERDE);

		if(ntx == 0)
		{

			ciaaPOSIX_memcpy(buffer[3], buffer[2], 128);
			ciaaPOSIX_memcpy(buffer[2], buffer[1], 128);
			ciaaPOSIX_memcpy(buffer[1], buffer[0], 128);
			ciaaPOSIX_memset(buffer[0], '\0', 128);

			GetResource(ResourceVariablesMision);
			{
				VariablesMision_CalcularPromedio(&promedio);

				promedio.nroMedicion = contadorMisionPrimaria;

				Decimal_a_Binario(&binario, &promedio);

				ArmarTrama(buffer[0], msgTramaMision, &promedio);

				if(temperaturaSecundaria > 0)
				{
					sprintf_float(bufferSecundario, msgTramaSecundaria, temperaturaSecundaria);
					ciaaPOSIX_strcat(buffer[0], bufferSecundario);
					temperaturaSecundaria = 0;
				}
			}
			ReleaseResource(ResourceVariablesMision);

//			LED_PARPADEAR(LED_2);
//			LED_ENCENDER(LED_2);
			GetResource(ResourceUART);
			{
				RADIO_OUT(buffer[0]);
//				DEBUG_OUT(buffer[0]);
			}
			ReleaseResource(ResourceUART);
//			LED_APAGAR(LED_2);
//			LED_PARPADEAR(LED_2);


			if(sae.estado_memoria == SAE_DISPOSITIVO_MEMORIA_OK)
			{
				if(	sae.estado_memoria == SAE_DISPOSITIVO_MEMORIA_OK &&
					sae.estado_memoria_disponible != SAE_DISPOSITIVO_MEMORIA_LLENA)
				{
					memdir = sensores.direccion + MEMORIA_DATO_LON * contadorMisionPrimaria;

					GetResource(ResourceBusI2C);
					{
						AT24C512_Direccion(memdir);
//						LED_ENCENDER(LED_2);
						if(AT24C512_Grabar((uint8_t*)&binario, MEMORIA_DATO_LON) != 1)
						{
							sae.estado_memoria = SAE_DISPOSITIVO_MEMORIA_ERROR;
						}
//						LED_APAGAR(LED_2);
					}
					ReleaseResource(ResourceBusI2C);

					if((memdir + MEMORIA_DATO_LON) < MEMORIA_DATO_DIR)
					{
						sae.estado_memoria_disponible = SAE_DISPOSITIVO_MEMORIA_LLENA;
					}
				}
			}

			contadorMisionPrimaria++;
			ntx++;
		}
		else
		{
			if(ntx <= N_RETRANSMISIONES)
			{
//				LED_ENCENDER(LED_2);
				GetResource(ResourceUART);
				{
					RADIO_OUT(buffer[ntx]);
//					DEBUG_OUT(buffer[ntx]);
				}
				ReleaseResource(ResourceUART);
//				LED_APAGAR(LED_2);
			}
			ntx++;

			if(ntx >= Nmax_TRANSMISIONES_xTRAMA)
			{
				ntx = 0;
			}
		}

		LED_PARPADEAR(LED_2);
		DEBUG_OUT(msgPrimaria_Fin);

	}
	else
	{
		LED_PARPADEAR(LEDCOLOR_VERDE);
	}



	TerminateTask();
}

/** \brief Mision secundaria
 *
 *  Similar a la mision primaria pero con menor prioridad.
 *
 * 	Se ejecuta periodicamente cada 1 segundo.
 *
 */
TASK(TareaMisionSecundaria)
{
	static bool			inicializado 			= false;
	float temp;
	int i;

	uint32_t x, y, z, a;

	if(!inicializado)
	{
		inicializado = true;

		SetRelAlarm(AlarmaMisionSecundaria, 1000, 1000);
//		SetRelAlarm(AlarmaMisionSecundaria, 1000, 0);
	}

	if(sae.estado_dispositivo == SAE_DISPOSITIVO_INICIALIZADO  && nroMedicionMision > 10)
	{
		DEBUG_OUT(msgSecundaria_Inicio);

//		LED_ENCENDER(LED_3);
		GetResource(ResourceVariablesMision);
		{
			temp = 0;
			for(i = 0; i < Nmax_MEDICIONES_xCICLO; i++)
			{
				temp += variablesMision[i].temperatura_aux;
			}
			temperaturaSecundaria = temp / Nmax_MEDICIONES_xCICLO;

//			for(x = 0; x < 0xffffff; x++)
//			{
//				a = a + 1;
//				for(y = 0; y < 0xffffff; y++)
//				{
//					a = 10 * a;
//					for(z = 0; z < 0xffffff; z++)
//					{
//						a--;
//					}
//				}
//			}
		}
		ReleaseResource(ResourceVariablesMision);
//		LED_APAGAR(LED_3);

		contadorMisionSecundaria++;
		LED_PARPADEAR(LED_3);
		DEBUG_OUT(msgSecundaria_Fin);

//		SetRelAlarm(AlarmaMisionSecundaria, 1000, 0);
	}

	TerminateTask();
}

/*
 * Tarea LeerSensores:
 *
 * Se ejecuta cada 100ms.
 * Toma una medicion de cada sensor y las aloja en un vector de mediciones crudas.
 * Luego las tareas de mision primaria y secundaria las acceden cada 1 segundo.
 */
TASK(TareaLeerSensores)
{
	static bool			inicializado 			= false;
//	static char			buffer[128];
	VariablesMision		medicionActual;
	int					ret;
	uint8_t 			teclas;

	if(!inicializado)
	{
		inicializado = true;

		/*
		 * Inicio WatchDog
		 */
		Chip_WWDT_Init(LPC_WWDT);
		Chip_WWDT_SetTimeOut(LPC_WWDT, (2*2) * (WDT_OSC/10)/4); // 15 segundos
		Chip_WWDT_SetWindow(LPC_WWDT, 0xFFFFFF); // sin ventana
		Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET);
		Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF | WWDT_WDMOD_WDINT);
		Chip_WWDT_Start(LPC_WWDT);

		SetRelAlarm(AlarmaCicloSensores, 100, 100);
	}

	if(sae.estado_dispositivo == SAE_DISPOSITIVO_INICIALIZADO)
	{
//		DEBUG_OUT(msgSensores_Inicio);

		GetResource(ResourceBusI2C);
		{
			ret = Medicion();
		}
		ReleaseResource(ResourceBusI2C);

		if(ret == MEDICION_OK)
		{
//			LED_ENCENDER(LED_1);
//			sprintf_float(buffer, msgSensores_Medicion, nroMedicionMision);
//			DEBUG_OUT(buffer);

			medicionActual.presion_Pascal			= medicion.dato.presion;
			medicionActual.temperatura_Centigrados 	= medicion.dato.temperatura;
			medicionActual.aceleracion_x			= medicion.dato.acel[0] * GRAVEDAD;
			medicionActual.aceleracion_y			= medicion.dato.acel[1] * GRAVEDAD;
			medicionActual.aceleracion_z			= medicion.dato.acel[2] * GRAVEDAD;
			medicionActual.temperatura_aux			= medicion.dato.temp;

			nroMedicionActual = nroMedicionMision % Nmax_MEDICIONES_xCICLO;

			GetResource(ResourceVariablesMision);
			{
				variablesMision[nroMedicionActual] = medicionActual;
			}
			ReleaseResource(ResourceVariablesMision);

			nroMedicionMision++;
//			LED_APAGAR(LED_1);

			LED_PARPADEAR(LED_1);
		}

		contadorLeerSensores++;
//		DEBUG_OUT(msgSensores_Fin);
	}


//	ciaaPOSIX_read(teclado, &teclas, 1);
//	if(!(teclas & TECLA_2))
//	{
		if(1 == vivoMisionPrimaria)
		{
			vivoMisionPrimaria = 0;
			Chip_WWDT_Feed(LPC_WWDT);
		}
//	}



	TerminateTask();
}
