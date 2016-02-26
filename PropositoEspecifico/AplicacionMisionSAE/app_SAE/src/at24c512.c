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

/**
 * @file	at24c5215.c
 * @author	SUR Emprendimientos Tecnologicos S.R.L.
 * @date	Mayo, 2015
 * @brief	Driver AT24C512 - Portable.
 *
 * Funciónes de la biblioteca de driver para memoria EEPROM AT24C512
 */

/*
 * at24c5215.c
 */

/**
 * @addtogroup Memoria
 * @{
 */

/**
 * @addtogroup Funciones
 * @{
 */

#include "../inc/at24c512.h"

#define _AT24C512_CAPACIDAD			(512*1024 / 8)										///< Capacidad en bytes de la memoria
#define _AT24C512_PAGINA_CAPACIDAD	(128) 												///< Capacidad en bytes de la página
#define _AT24C512_PAGINAS			(_AT24C512_CAPACIDAD / _AT24C512_PAGINA_CAPACIDAD)	///< Cantidad de páginas
#define _AT24C512_MASCARA			((uint32_t)(_AT24C512_CAPACIDAD - 1))				///< Máscara de direcciones 0xFFFFFF

/**
 * @brief	Estructura de abstracción del dispositivo AT24C512
 */
typedef struct
{
	uint8_t pagina_actual;			///< Indice de última pagina accedida
	uint8_t pagina_disponible;		///< Cantidad de páginas disponibles
	uint16_t direccion;
} at24c512_data_u;

at24c512_data_u at24c512;

int AT24C512_Inicializar(void)
{
	at24c512.direccion = 0x00000000;
	return 1;
}

int AT24C512_Direccion(uint16_t direccion)
{
	at24c512.direccion = direccion;
	return 1;
}

int AT24C512_Grabar(uint8_t * pDatos, uint8_t cantidad)
{
	uint8_t out_buf[_AT24C512_PAGINA_CAPACIDAD + 2];
	int i;
	int len;

	// no escribir más que la capacidad de una pagina
	if(_AT24C512_PAGINA_CAPACIDAD < cantidad)
	{
		return 0;
	}
	at24c512.pagina_actual = at24c512.direccion / _AT24C512_PAGINA_CAPACIDAD;
	at24c512.pagina_disponible = _AT24C512_PAGINA_CAPACIDAD - at24c512.direccion % _AT24C512_PAGINA_CAPACIDAD;

	// no superar el máximo de la pagina actual
	if(at24c512.pagina_disponible < cantidad)
	{
		return 0;
	}

	// no superar el máximo de la memoria
	if(_AT24C512_MASCARA  < (at24c512.direccion + cantidad -1))
	{
		return 0;
	}

	out_buf[0] = (uint8_t)((at24c512.direccion >> 8) & 0x00FF);
	out_buf[1] = (uint8_t)((at24c512.direccion >> 0) & 0x00FF);

	for(i = 0; i < cantidad; i++)
	{
		out_buf[2+i] = pDatos[i];
	}

	len = AT24C512_I2C_Escribir( AT24C512_I2C_ADDRESS, out_buf, cantidad + 2 );
	at24c512.direccion += cantidad;

	if(len == (cantidad + 2))
	{
		return 1;
	}
	return -1;
}

int AT24C512_Leer(uint8_t * pDatos, uint8_t cantidad)
{
	uint8_t out_buf[2];
	int len = 0;

	// no escribir más que la capacidad de una pagina
	if(_AT24C512_PAGINA_CAPACIDAD < cantidad)
	{
		return 0;
	}
	at24c512.pagina_actual = at24c512.direccion / _AT24C512_PAGINA_CAPACIDAD;
	at24c512.pagina_disponible = _AT24C512_PAGINA_CAPACIDAD - at24c512.direccion % _AT24C512_PAGINA_CAPACIDAD;
	// no superar el máximo de la pagina actual
	if(at24c512.pagina_disponible < cantidad)
	{
		return 0;
	}
	// no superar el máximo de la memoria
	if(_AT24C512_MASCARA  < (at24c512.direccion + cantidad -1))
	{
		return 0;
	}

	out_buf[0] = (uint8_t)((at24c512.direccion >> 8) & 0x00FF);
	out_buf[1] = (uint8_t)((at24c512.direccion >> 0) & 0x00FF);

	len = AT24C512_I2C_Leer( AT24C512_I2C_ADDRESS, out_buf, 2, pDatos, cantidad );
	at24c512.direccion += cantidad;

	if(len == cantidad)
	{
		return 1;
	}
	return -1;
}

/** @}*/

/** @}*/
