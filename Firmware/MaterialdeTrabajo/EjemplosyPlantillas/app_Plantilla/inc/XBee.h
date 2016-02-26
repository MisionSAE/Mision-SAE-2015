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

#ifndef XBEE_H_
#define XBEE_H_

#include "Types.h"

#define	XBEE_TAMANIO_LINEA		256							//! Tamaño del buffer
#define	STRING_IGUALES			0							//! Constante para la comparación de strings

#define	XBEE_USE_PHYS_ADDRESS	0xffff						//! Dirección a pasar a XBeeSetOwnAddress para trabajar con direcciones físicas

#define XBEE_BAUD_1200			0							//! 1200bps
#define XBEE_BAUD_2400			1							//! 2400bps
#define XBEE_BAUD_4800			2							//! 4800bps
#define XBEE_BAUD_9600			3							//! 9600bps
#define XBEE_BAUD_19200			4							//! 19200bps
#define XBEE_BAUD_38400			5							//! 38400bps
#define XBEE_BAUD_57600			6							//! 57600bps
#define XBEE_BAUD_115200		7							//! 115200bps

#define XBEE_CANAL_MINIMO		0x0b						//! Mínimo canal soportado por el módulo
#define XBEE_CANAL_MAXIMO		0x1a						//! Máximo canal soportado por el módulo

bool		XBeeIniciar					(int fileDescriptor);
bool		XBeeCerrar					();

bool		XBeeEntrarModoComandos		(int* contadorEtapa);
bool		XBeeSalirModoComandos		();

bool		XBeeTransmitirDatos			(const char* datos, uint8_t cantidad);
bool		XBeeLeerLinea				(char* destino, int* posicion);

bool		XBeeEsOK					(const char* destino);
bool		XBeeEsERROR					(const char* destino);

uint64_t	XBeeGetSerial				();
uint16_t	XBeeGetFirmwareVersion		();
const char*	XBeeGetVersionTextual		();
int			XBeeGetPotenciaRecepcion	();

bool		XBeeSetChannel				(int canal);
bool		XBeeSetBaudRate				(int baudRate);
bool		XBeeSetOwnAddress			(uint16_t newAddress);
uint16_t	XBeeGetOwnAddress			();
bool		XBeeSetPANID				(uint16_t PAN_ID);
uint16_t	XBeeGetPANID				();
bool		XBeeSetDestinationAddress	(uint16_t newAddress);
uint64_t	XBeeGetDestinationAddress	();

bool		XBeeSaveConfig				();
bool		XBeeReset					();

#endif /* XBEE_H_ */
