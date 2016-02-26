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
 * Archivos necesarios para ejecutar el ejemplo.
 * Son parte del CIAA-Firmware, no entraremos en detalle
 */
#include "../inc/main.h"

#include "os.h"
#include "ciaaPOSIX_stdio.h"
#include "ciaaPOSIX_string.h"
#include "ciaak.h"

/*
 * Antes de continuar leyendo, revisar el contenido del archivo: "../inc/main.h"
 */

/*
 * [!] TIP:
 * Para una mejor organización del firmware es recomendable trabajar
 * el estado de las entradas y salidas primero en varibles
 * y al finalizar el proceso enviar las ordenes al HW.
 */
uint8_t	estado_leds;

/*
 * Descriptores de HW, se usan para identificar el HW y operar sobre el mismo.
 */
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

	ciaaPOSIX_ioctl(fd_uart_usd, ciaaPOSIX_IOCTL_SET_BAUDRATE, (void *)ciaaBAUDRATE_115200);
	ciaaPOSIX_ioctl(fd_uart_usd, ciaaPOSIX_IOCTL_SET_FIFO_TRIGGER_LEVEL, (void *)ciaaFIFO_TRIGGER_LEVEL3);

	/*
	 * En este lugar es recomendable inicilizar y configurar los dispositivos,
	 * antes que comienze la aplicación
	 */

	Periodic_Task_Counter = 0;
	SetRelAlarm(ActivatePeriodicTask, BASETIEMPO, BASETIEMPO);
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

#define EJEMPLO_MODO_SIMPLE				(1)
#define EJEMPLO_MODO_USANDO_DEFINES		(2)
#define EJEMPLO_MODO					(EJEMPLO_MODO_USANDO_DEFINES)

#if EJEMPLO_MODO == EJEMPLO_MODO_SIMPLE
TASK(PeriodicTask)
{
	static int HW_inicializado = NO;
	static int contador = 0;

	if ( HW_inicializado == NO )
	{
		// Esta sección de código es una pequeñá rutina de inicialización, solo se ejecuta 1 vez
		estado_leds = 0x00;
		HW_inicializado = SI;
	}
	else
	{
		contador++;

		// [!] TIP: es recomendable preguntar por igual mayor o igual menor
		if ( contador >= (1000/BASETIEMPO) )
		{
			contador = 0;
		}

		// [!] TIP: el operador % devuelve el resto de la división
		if ( !( contador % (500/BASETIEMPO)) )
		{
			estado_leds |= mascara(RGB_R_bit);
			estado_leds |= mascara(LED_1_bit);

			estado_leds &= ~mascara(RGB_G_bit);
			estado_leds &= ~mascara(LED_2_bit);

			estado_leds ^= mascara(RGB_B_bit);
			estado_leds ^= mascara(LED_3_bit);
		}
	}

	// Esta instrucción es la única parte del firmware que accede al HW
	ciaaPOSIX_write(fd_leds, &estado_leds, 1);

	TerminateTask();
}
#endif

#if EJEMPLO_MODO == EJEMPLO_MODO_USANDO_DEFINES

/*
 * Las máscaras permiten trabajar de forma práctica a nivel de bits.
 * Su principal función es la de operar solo con los bits de interes sin afectar al resto de los bits
 *
 * Ej.:
 * Se tiene el siguiente byte:
 * uint8_t VALOR = b00001111;
 * o también puede escribirse
 * uint8_t VALOR = 0x0F;
 * Se desea operar con el bit 1, colocandolo en 0
 * Se utiliza para eso una máscara:
 * uint8_t MASCARA = b11111101
 * Realizando la operación AND, se obtendra = b00001101
 * uint8_t RESULTADO = VALOR & MASCARA
 *
 * NOTA: el prefijo b en un número indica que se debe interpretar como un valor binario,
 * mientras que el prefijo 0x o 0X indica que se debe interpretar como un valor hexadecimal
 */
#define mascara(n)	((uint8_t)(1<<(n)))

#define	RGB_R					(mascara(RGB_R_bit))
#define	RGB_G					(mascara(RGB_G_bit))
#define	RGB_B					(mascara(RGB_B_bit))
#define	LED_1					(mascara(LED_1_bit))
#define	LED_2					(mascara(LED_2_bit))
#define	LED_3					(mascara(LED_3_bit))
#define LED_TODOS				(RGB_R | RGB_G | RGB_B | LED_1 | LED_2 | LED_3)

#define	GPIO_SET(est,mas)		est |= (mas)
#define	GPIO_CLEAR(est,mas)		est &= ~(mas)
#define	GPIO_TOOGLE(est,mas)	est ^= (mas)

TASK(PeriodicTask)
{
	static int HW_inicializado = NO;
	static int contador = 0;

	if ( HW_inicializado == NO )
	{
		GPIO_CLEAR(estado_leds, LED_TODOS);
		HW_inicializado = SI;
	}
	else
	{
		contador++;

		if ( contador >= (1000/BASETIEMPO) )
		{
			contador = 0;
		}

		if ( !( contador % (500/BASETIEMPO)) )
		{
			GPIO_SET(estado_leds, LED_1);
			GPIO_CLEAR(estado_leds, LED_2);
			GPIO_TOOGLE(estado_leds, LED_3);
		}

		if ( !( contador % (100/BASETIEMPO)) )
		{
			GPIO_TOOGLE(estado_leds, RGB_R);
		}

		if ( !( contador % (200/BASETIEMPO)) )
		{
			GPIO_TOOGLE(estado_leds, RGB_G);
		}

		if ( !( contador % (400/BASETIEMPO)) )
		{
			GPIO_TOOGLE(estado_leds, RGB_B);
		}
	}

	ciaaPOSIX_write(fd_leds, &estado_leds, 1);

	TerminateTask();
}
#endif
