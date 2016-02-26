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

#ifndef INC_PLATAFORMA_H_
#define INC_PLATAFORMA_H_

/****************************************************************
 * DEFINICIONES GENERALES										*
 ****************************************************************/

#define SI				(1)
#define NO				(0)

#define mascara(n)		((uint8_t)(1<<(n)))

/****************************************************************
 * PINOUT														*
 ****************************************************************/

/*
 * /dev/dio/in/0
 * bits		| 7	| 6	| 5	| 4	| 3	| 2	| 1	| 0	|
 * LEDs		| a	| b | 3	| 2	| 1	| B	| G	| R	|
 */
#define	RGB_R_bit		(0)
#define	RGB_G_bit		(1)
#define	RGB_B_bit 		(2)
#define	LED_1_bit		(3)
#define	LED_2_bit		(4)
#define	LED_3_bit		(5)
#define	RGB_R			(mascara(RGB_R_bit))
#define	RGB_G			(mascara(RGB_G_bit))
#define	RGB_B			(mascara(RGB_B_bit))
#define	LED_1			(mascara(LED_1_bit))
#define	LED_2			(mascara(LED_2_bit))
#define	LED_3			(mascara(LED_3_bit))
#define LED_TODOS		(RGB_R | RGB_G | RGB_B | LED_1 | LED_2 | LED_3)

/*
 * /dev/dio/out/0
 * bits		| 7	| 6	| 5	| 4	| 3	| 2	| 1	| 0	|
 * LEDs		| a	| b | c	| d	| 4	| 3	| 2	| 1	|
 */
#define	TECLA_1_bit		(0)
#define	TECLA_2_bit		(1)
#define	TECLA_3_bit 	(2)
#define	TECLA_4_bit		(3)
#define	TECLA_1			(mascara(TECLA_1_bit))
#define	TECLA_2			(mascara(TECLA_2_bit))
#define	TECLA_3			(mascara(TECLA_3_bit))
#define	TECLA_4			(mascara(TECLA_4_bit))
#define TECLA_TODAS		(TECLA_1 | TECLA_2 | TECLA_3 | TECLA_4)

/****************************************************************
 * GPIO															*
 ****************************************************************/

#define	GPIO_SET(est,mas)		est |= (mas)
#define	GPIO_CLEAR(est,mas)		est &= ~(mas)
#define	GPIO_TOOGLE(est,mas)	est ^= (mas)
#define	GPIO_GET(est,mas)		est & (mas)

#endif /* INC_PLATAFORMA_H_ */
