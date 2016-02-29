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

/** \brief Mision primaria
 *
 * 	Luego se ejecuta periodicamente cada 1 segundo.
 *
 */
TASK(TareaMisionPrimaria)
{
	static bool				inicializado 			= false;

	if(!inicializado)
	{
		inicializado = true;

		/*
		 * PROCESO QUE SE EJECUTA POR ÚNICA VEZ --->
		 */

		/*
		 *  <--- FIN DEL PROCESO UNICO
		 */

		SetRelAlarm(AlarmaMisionPrimaria, 1000, 1000);
	}

	/*
	 * PROCESO PRINCIPAL --->
	 */

	/*
	 *  <--- FIN DEL PROCESO PRINCIPAL
	 */

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

	if(!inicializado)
	{
		inicializado = true;

		/*
		 * PROCESO QUE SE EJECUTA POR ÚNICA VEZ --->
		 */

		/*
		 *  <--- FIN DEL PROCESO UNICO
		 */

		SetRelAlarm(AlarmaMisionSecundaria, 1000, 1000);
	}

	/*
	 * PROCESO PRINCIPAL --->
	 */

	/*
	 *  <--- FIN DEL PROCESO PRINCIPAL
	 */

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

	if(!inicializado)
	{
		inicializado = true;

		/*
		 * PROCESO QUE SE EJECUTA POR ÚNICA VEZ --->
		 */

		/*
		 *  <--- FIN DEL PROCESO UNICO
		 */

		SetRelAlarm(AlarmaCicloSensores, 100, 100);
	}

	/*
	 * PROCESO PRINCIPAL --->
	 */

	/*
	 *  <--- FIN DEL PROCESO PRINCIPAL
	 */

	TerminateTask();
}
