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

/** \brief Main function
 *
 */
int main(void)
{
	StartOS(AppModeMision);

	return 0;
}

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

	TerminateTask();
}

/** \brief Error Hook function
 *
 */
void ErrorHook(void)
{
	static char 		buffer[128];

	ciaaPOSIX_printf("ErrorHook was called\n");
	ciaaPOSIX_printf("Service: %d, P1: %d, P2: %d, P3: %d, RET: %d\n", OSErrorGetServiceId(), OSErrorGetParam1(), OSErrorGetParam2(), OSErrorGetParam3(), OSErrorGetRet());
	ShutdownOS(0);
}
