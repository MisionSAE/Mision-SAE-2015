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

#ifndef _MAIN_H_
#define _MAIN_H_

/*
 * [!] Recordatorio
 *
 * /dev/dio/in/0
 *
 * Relación entre bits y leds
 *
 * bits		| 7	| 6	| 5	| 4	| 3	| 2	| 1	| 0	|
 * LEDs		| a	| b | 3	| 2	| 1	| B	| G	| R	|
 */

/*
 * Para no usar "números mágicos en el firmware es mejor crear definiciones
 */
#define	RGB_R_bit	(0)
#define	RGB_G_bit	(1)
#define	RGB_B_bit 	(2)
#define	LED_1_bit	(3)
#define	LED_2_bit	(4)
#define	LED_3_bit	(5)

/*
 * Definimos una base de tiempo para ejecutar la aplicación
 */
#define BASETIEMPO	(10) // 100[milisegundos] = 0.001[segundos]

/*
 * Agrego todas definiciones que haga más claro el código
 */
#define SI	(1)
#define NO	(0)

#endif /* #ifndef _MAIN_H_ */

/*
 * Siempre dejar un espaci vacio al finalizar un archivo .h
 * ¿Por qué? TODO: completar !
 */
