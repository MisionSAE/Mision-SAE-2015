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

#ifndef _DEFINICIONES_H_
#define _DEFINICIONES_H_

/***********************************************************************
 *
 ***********************************************************************/

#include "os.h"
#include "ciaaPOSIX_stdio.h"
#include "ciaaPOSIX_string.h"
#include "ciaak.h"

/***********************************************************************
 *
 ***********************************************************************/

#define SAE_OK				(1)

/*
 * Memoria
 */

#define MEMORIA_CAPACIDAD					(512*1024/8)
#define MEMORIA_RESERVADO					((uint8_t)(0xff))
#define MEMORIA_PAGINA_CAPACIDAD			(128)
#define MEMORIA_PAGINA_CANTIDAD				(MEMORIA_CAPACIDAD / MEMORIA_PAGINA_CAPACIDAD)
#define MEMORIA_ENCABEZADO_DIR				((uint16_t)(0X0000))
#define MEMORIA_ENCABEZADO_LON				(64)
#define MEMORIA_ENCABEZADO_MARCA			((uint8_t)(0x87))
#define MEMORIA_ENCABEZADO_RESERVADO		((uint8_t)(0Xaa))
#define MEMORIA_DATO_DIR					(MEMORIA_ENCABEZADO_DIR + MEMORIA_ENCABEZADO_LON)
#define MEMORIA_DATO_LON					(32)
#define MEMORIA_DATO_MARCA					((uint8_t)(0xbb))
#define MEMORIA_DATO_RESERVADO				(MEMORIA_DATO_MARCA)

//#define MEMORIA_PAGINA_CANTIDAD				(MEMORIA_CAPACIDAD / MEMORIA_PAGINA_CAPACIDAD - 1)
//#define MEMORIA_RESERVADO					((uint8_t)MEMORIA_DATO_MARCA)

struct memoria_encabezado_s
{
	uint8_t		marca;
	uint8_t		coeficientes[22];
	uint8_t		contador;
	uint8_t		reservados[MEMORIA_ENCABEZADO_LON - (22 + 3)];
	uint8_t		crc;
} __attribute__((packed));

typedef struct memoria_encabezado_s memoria_encabezado_u;

struct memoria_dato_s
{
	uint16_t	nroMedicion;
	float		temperatura_Centigrados;
	float 		presion_Pascal;
	float		aceleracion_x;
	float		aceleracion_y;
	float		aceleracion_z;
	uint8_t		reservados[MEMORIA_DATO_LON - 20 - 2 - 2];
	uint8_t		crc;
	uint8_t		marca;
} __attribute__((packed));

typedef struct memoria_dato_s memoria_dato_u;

/***********************************************************************
 *
 ***********************************************************************/

/*
 * Valor del dip switch
 */
#define MODO_MISION_VALOR				(0x0f)
#define MODO_RECUPERACION_VALOR			(0x02)
#define MODO_AUTODIAGNOSTICO_VALOR		(0x08)
#define MODO_BORRADO_VALOR				(0x0c)
#define Nmax_TRANSMISIONES_xTRAMA		(4)
#define N_RETRANSMISIONES				(3)

/***********************************************************************
 *
 ***********************************************************************/

/*
 * TECLAS y LEDS
 */
#define	CANTIDAD_TECLAS				(4)
#define	TECLA_1						(1<<0)
#define	TECLA_2						(1<<1)
#define	TECLA_3						(1<<2)
#define	TECLA_4						(1<<3)
#define	LED_0_R 					(1<<0)
#define	LED_0_G 					(1<<1)
#define	LED_0_B 					(1<<2)
#define	LED_1						(1<<3)
#define	LED_2						(1<<4)
#define	LED_3						(1<<5)
#define	LED_TODOS					(0x3f)
#define	LED_ENCENDER(v)				sae.leds |= (v); \
									ciaaPOSIX_write(display, &sae.leds, 1)
#define	LED_APAGAR(v)				sae.leds &= ~(v); \
									ciaaPOSIX_write(display, &sae.leds, 1)
#define	LED_PARPADEAR(v)			sae.leds ^= (v); \
									ciaaPOSIX_write(display, &sae.leds, 1)

#define LEDCOLOR_ROJO				(LED_0_R)
#define LEDCOLOR_VERDE				(LED_0_G)
#define LEDCOLOR_AZUL				(LED_0_B)
#define LEDCOLOR_AMARILLO			(LED_0_R | LED_0_G)
#define LEDCOLOR_ROSA				(LED_0_R | LED_0_B)
#define LEDCOLOR_CELESTE			(LED_0_G | LED_0_B)
#define LEDCOLOR_BLANCO				(LED_0_R | LED_0_G | LED_0_B)


#define	LEDCOLOR_ENCENDER(v)		sae.leds |= colores[(v)].valorPin; \
									ciaaPOSIX_write(display, &sae.leds, 1)
#define	LEDCOLOR_APAGAR(v)			sae.leds &= ~(colores[(v)].valorPin); \
									ciaaPOSIX_write(display, &sae.leds, 1)
#define	LEDCOLOR_PARPADEAR(v)		sae.leds ^= ~(colores[(v)].valorPin); \
									ciaaPOSIX_write(display, &sae.leds, 1)


typedef	unsigned int 	uint;



char* sprintf_float(char* buffer, const char* formato, float dato);

#define DEBUG_OUT(x)				ciaaPOSIX_write(uartUSB, (x), ciaaPOSIX_strlen(x))
#define RADIO_OUT(x)				XBeeTransmitirDatos((x), ciaaPOSIX_strlen((x)))





//static uint8_t	estadoAnterior	[CANTIDAD_TECLAS];
//static uint8_t	estadoActual	[CANTIDAD_TECLAS];
static const char	msgPresionada[]			=	"Presionada";
static const char	msgSoltada[]			=	"Soltada";
static const char	msgSeparador[]			=	" ";
static const char	msgFinDeLinea[]			=	"\r\n";

struct Comando
{
	const char*	nombre;
	char			tecla;
	uint8_t		valorPin;
};

extern struct Comando	colores[];
extern struct Comando	leds[];
extern struct Comando  teclas[CANTIDAD_TECLAS];

typedef struct StructVariablesMision
{
	uint		nroMedicion;
	float		temperatura_Centigrados;
	float 		presion_Pascal;
	float		aceleracion_x;
	float		aceleracion_y;
	float		aceleracion_z;
	float		temperatura_aux;
} VariablesMision;

/*
 * Globales
 */
#define Nmax_MEDICIONES_xCICLO 	(10)
#define GRAVEDAD 				(9.81)

/*
 * funciones
 */
//void 		CalcularPromedio		(void);
void 		Decimal_a_Binario		(memoria_dato_u * pBinario, VariablesMision * pDecimal);
void 		Binario_a_Decimal		(VariablesMision * pDecimal, memoria_dato_u * pBinario);
void 		ArmarTrama				(char* buffer, char* trama, VariablesMision * pVariables);
uint8_t 	CalcularCRC				(uint8_t * pData, int len);

#endif /* #ifndef _DEFINICIONES_H_ */
