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

/*
 * Antes de continuar leyendo, revisar los cambios en: "../inc/main.h" y "../inc/plantilla.h"
 */
#include "../inc/main.h"

#include "os.h"
#include "ciaaPOSIX_stdio.h"
#include "ciaaPOSIX_string.h"
#include "ciaak.h"

uint8_t	estado_leds;
uint8_t	estado_teclas;

static int32_t fd_teclado;
static int32_t fd_leds;
static int32_t fd_uart_usd;
static int32_t fd_uart_expansion;

static uint32_t Periodic_Task_Counter;

int main(void)
{
	StartOS(AppMode1);
	return 0;
}

TASK(InitTask)
{
	ciaak_start();

	ciaaPOSIX_printf("Init Task...\n");

	fd_teclado 			= ciaaPOSIX_open("/dev/dio/in/0", O_RDONLY);
	fd_leds				= ciaaPOSIX_open("/dev/dio/out/0", O_RDWR);
	fd_uart_usd			= ciaaPOSIX_open("/dev/serial/uart/1", O_RDWR);
	fd_uart_expansion	= ciaaPOSIX_open("/dev/serial/uart/2", O_RDWR);

	ciaaPOSIX_ioctl(fd_uart_usd, ciaaPOSIX_IOCTL_SET_BAUDRATE, (void *)ciaaBAUDRATE_57600);
	ciaaPOSIX_ioctl(fd_uart_usd, ciaaPOSIX_IOCTL_SET_FIFO_TRIGGER_LEVEL, (void *)ciaaFIFO_TRIGGER_LEVEL3);

	ciaaPOSIX_ioctl(fd_uart_expansion, ciaaPOSIX_IOCTL_SET_BAUDRATE, (void *)ciaaBAUDRATE_57600);
	ciaaPOSIX_ioctl(fd_uart_expansion, ciaaPOSIX_IOCTL_SET_FIFO_TRIGGER_LEVEL, (void *)ciaaFIFO_TRIGGER_LEVEL3);

	GPIO_CLEAR(estado_leds, LED_TODOS);

	Periodic_Task_Counter = 0;
	SetRelAlarm(ActivatePeriodicTask, BASETIEMPO, BASETIEMPO);
	ActivateTask(SerialEchoTask);

	TerminateTask();
}

/*
 * Por ahora no vamos a entrar en detalle sobre esta función
 * La veremos más adelante
 */
void ErrorHook(void)
{
	ciaaPOSIX_printf("ErrorHook was called\n");
	ciaaPOSIX_printf("Service: %d, P1: %d, P2: %d, P3: %d, RET: %d\n", OSErrorGetServiceId(), OSErrorGetParam1(), OSErrorGetParam2(), OSErrorGetParam3(), OSErrorGetRet());
	ShutdownOS(0);
}

TASK(SerialEchoTask)
{
	int8_t buf[20];   /* buffer for uart operation              */
	int32_t ret;      /* return value variable for posix calls  */

	char mensaje[] = "Mensaje enviado por UART\n";
	ciaaPOSIX_write(fd_uart_usd, mensaje, ciaaPOSIX_strlen(mensaje));
	while(1)
	{
		ret = ciaaPOSIX_read(fd_uart_usd, buf, 20);
		if(ret > 0)
		{
			ciaaPOSIX_write(fd_uart_usd, buf, ret);
		}
	}
}


TASK(PeriodicTask)
{
	LeerEntradas();

	Procesar();

	EscribirSalidas();

	TerminateTask();
}

int LeerEntradas(void)
{
	ciaaPOSIX_read(fd_teclado, &estado_teclas, 1);
	ciaaPOSIX_read(fd_leds, &estado_leds, 1);
	return 1;
}

int Procesar(void)
{
	static int contador = 0;
	static int barrido;

	contador++;

	if ( GPIO_GET(estado_teclas, TECLA_2) )
		GPIO_SET(estado_leds, LED_1);
	else
		GPIO_CLEAR(estado_leds, LED_1);

	if ( GPIO_GET(estado_teclas, TECLA_3) )
		GPIO_SET(estado_leds, LED_2);
	else
		GPIO_CLEAR(estado_leds, LED_2);

	if ( GPIO_GET(estado_teclas, TECLA_4) )
		GPIO_SET(estado_leds, LED_3);
	else
		GPIO_CLEAR(estado_leds, LED_3);

	if ( GPIO_GET(estado_teclas, TECLA_1) )
	{
		barrido++;
		barrido &= 0x07;
		GPIO_CLEAR(estado_leds, RGB_R | RGB_G | RGB_B);
		GPIO_SET(estado_leds, barrido);
		/*
		 * Se visualiza el cambio?
		 * Problemas?
		 * Implementación de una máquina de estados
		 */
	}

	return 1;
}

int EscribirSalidas(void)
{
	ciaaPOSIX_write(fd_leds, &estado_leds, 1);
	return 1;
}
